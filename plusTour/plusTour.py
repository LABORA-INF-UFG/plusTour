import json
from docplex.mp.model import Model
from docplex.util.environment import get_environment
import random
from timeit import default_timer as timer
import os
import argparse

net_cap = 300

POI_LIST = []

def read_cities_to_solve(data_file):
    list_of_cities = []
    
    file = open(data_file, "r")
    data = file.readlines()
    
    for i in data:
        if i[0] == '#':continue
        list_of_cities.append(i.rstrip())
    print(list_of_cities)    
    return list_of_cities

def poi_list(data_file):
    global POI_LIST
    POI_LIST = []
    file = open(data_file, "r")
    data = file.readlines()
    
    for i in data:
        POI_LIST.append(int(i.split()[0]))

def serialize(obj):
    return obj.__dict__


class Tour:
    def __init__(
        self,
        seq_id,
        user,
        user_code,
        id,
        path,
        original_seq,
        allocation,
        allocation_at_poi,
        profit,
        app_profile,
        min_bandwidth,
        max_bandwitdh,
        min_computational,
        max_computational,
    ):
        self.user = user
        self.user_code = user_code
        self.id = id
        self.allocation = allocation
        self.allocation_at_poi = allocation_at_poi
        self.profit = profit
        self.path = path
        self.original_seq = original_seq
        self.app_profile = app_profile
        self.min_bandwidth = min_bandwidth
        self.max_bandwitdh = max_bandwitdh
        self.min_computational = min_computational
        self.max_computational = max_computational
        # self.max_tour_alloc = [1] * 480
        # self.min_tour_alloc = [1] * 480
        self.total_bandwidth_alloc = 0
        self.total_computational_alloc = 0
        self.visited = [None] + [-1] * 480
        self.visits = {}
        self.num_visits = 0
        self.bandwidth_allocation = {}
        self.computational_allocation = {}
        self.bandwidth_allocation_perstour = {}
        self.computational_allocation_perstour = {}
        self.seq_id = seq_id

        for poi in POI_LIST:
            self.visits[poi] = 0
            # if self.id == 259 and poi == 21:
            # 	print "YOOOLO"
            # 	print self.allocation_at_poi[poi]
            # 	print len(self.allocation_at_poi[poi])
            # 	exit()
            for alloc in range(1, 481):
                if int(self.allocation_at_poi[poi][alloc]) == 1:
                    self.visited[alloc] = poi
                    self.visits[poi] = 1
                    # self.min_tour_alloc[alloc] = self.min_alloc
                    # self.max_tour_alloc[alloc] = self.max_alloc
                    self.total_computational_alloc += self.max_computational
                    self.total_bandwidth_alloc += self.max_bandwitdh

            self.num_visits = self.num_visits + self.visits[poi]

    def __str__(self):
        return str(self.id)


class Application:
    def __init__(
        self, name, min_bandwidth, max_bandwitdh, min_computational, max_computational
    ):
        self.name = name
        self.min_bandwidth = min_bandwidth
        self.max_bandwitdh = max_bandwitdh
        self.min_computational = min_computational
        self.max_computational = max_computational


class AppProfile:
    def __init__(self, id):
        self.id = id
        self.applications = []

    def add_application(self, application):
        self.applications.append(application)


class ECCRU:
    def __init__(self, id, capacity):
        self.id = id
        self.capacity = capacity

    def __str__(self):
        return str(self.id)


class User:
    def __init__(self, id, tours, optimal_tour, profile):
        self.id = id
        self.tours = tours
        self.optimal_tour = optimal_tour
        self.profile = profile

    def __str__(self):
        return str(self.id)

def user_application_profile(filename = 'user_apps.in'):
    user_dict = {}
    with open(filename) as file:
        for line in file:
            options = line.rstrip().split('"')
            if options[1] == 'Augmented reality':
                options[1] = (2, options[1])
            elif options[1] == 'Video streaming':
                options[1] = (3, options[1])
            elif options[1] == 'No application':
                options[1] = (4, options[1])
            elif options[1] == 'Video streaming and augmented reality':
                options[1] = (1, options[1])
            user_seq_id = options[0].split()
            user_dict[(user_seq_id[0], int(user_seq_id[1]))] = options[1]

    #print(user_dict)
    return user_dict
    

def process_solutions(file_name):
    global users
    global num_sols
    global all_tours
    global profiles

    file = open(file_name, "r")
    data = file.readlines()

    line_num = 0
    num_users = int(data[line_num])
    line_num += 1
    users = []
    num_sols = 0

    for user in range(1, num_users + 1):
        user_code = data[line_num].rstrip()
        line_num += 1
        num_tours = int(data[line_num])
        tours = []
        line_num += 1
        '''
        app_profile = random.choice(profiles)

        min_bandwidth = 0
        max_bandwitdh = 0
        min_computational = 0
        max_computational = 0

        for app in app_profile.applications:
            min_bandwidth += app.min_bandwidth
            max_bandwitdh += app.max_bandwitdh
            min_computational += app.min_computational
            max_computational += app.max_computational
        '''
        for tour in range(num_tours):
            seq_id, original_seq = data[line_num].split()
            seq_id = int(seq_id)
            original_seq =  list(map(int, original_seq.split('-')))

            line_num += 1

            user_app_profile = user_application_profile()
            #print("this is returning")
            #return
            app_profile = profiles[user_app_profile[(user_code, seq_id)][0]-1]
            #app_profile = random.choice(profiles)

            min_bandwidth = 0
            max_bandwitdh = 0
            min_computational = 0
            max_computational = 0

            for app in app_profile.applications:
                min_bandwidth += app.min_bandwidth
                max_bandwitdh += app.max_bandwitdh
                min_computational += app.min_computational
                max_computational += app.max_computational            
            
            profit = float(data[line_num])
            line_num += 1
            path = []
            allocation = []
            allocation.append(None)
            allocation_at_poi = {}

            for visit in data[line_num].split():
                path.append(int(visit))

            line_num += 1

            for alloc in data[line_num].split():
                allocation.append(int(alloc))

            line_num += 1

            num_pois = int(data[line_num])

            line_num += 1

            for poi in POI_LIST:
                allocation_at_poi[poi] = []
                allocation_at_poi[poi].append(None)
                for alloc in data[line_num].split():
                    allocation_at_poi[poi].append(int(alloc))
                line_num += 1

            new_tour = Tour(
                seq_id,
                user,
                user_code,
                num_sols,
                path,
                original_seq,
                allocation,
                allocation_at_poi,
                profit,
                app_profile,
                min_bandwidth,
                max_bandwitdh,
                min_computational,
                max_computational,
            )
            all_tours.append(new_tour)
            tours.append(new_tour)
            num_sols += 1

        best_profit = -1
        best_tour = None

        for tour in tours:
            if tour.profit > best_profit:
                best_profit = tour.profit
                best_tour = tour

        optimal_tours.append(best_tour)

        users.append(User(user, tours, best_tour, app_profile))


def build_allocation_model():
    mdl = Model(name="Allocation Problem plusTour", log_output=True)
    # mdl.parameters.benders.strategy = 3
    y_tour = mdl.binary_var_dict(keys=(tour for tour in all_tours), name="y_tour")

    a_eccru = mdl.binary_var_cube(
        keys1=(tour for tour in all_tours),
        keys2=(poi for poi in POI_LIST),
        keys3=(eccru for eccru in eccrus),
        name="a_eccru",
    )

    rc_conectivity = mdl.continuous_var_matrix(
        keys1=(tour for tour in all_tours),
        keys2=(poi for poi in POI_LIST),
        lb=0,
        name="rc_conectivity",
    )

    rc_computational = mdl.continuous_var_cube(
        keys1=(tour for tour in all_tours),
        keys2=(poi for poi in POI_LIST),
        keys3=(eccru for eccru in eccrus),
        lb=0,
        name="rc_computational",
    )

    """
	eccrus = []

	for eccru in range(1, num_eccrus + 1):
		a_eccru = mdl.binary_var_dict(keys = (tour for tour in all_tours), name = 'a_eccru_{}'.format(eccru))
		eccrus.append(a_eccru)
	"""

    # Objective function
    mdl.maximize(
        mdl.sum(
            y_tour[tour] * tour.profit
            + y_tour[tour]
            * (
                mdl.sum(
                    tour.allocation_at_poi[poi][alloc]
                    * rc_computational[(tour, poi, eccru)]
                    for poi, eccru, alloc in [
                        (poi, eccru, alloc)
                        for poi in POI_LIST
                        for eccru in eccrus
                        for alloc in range(1, 481)
                    ]
                )
            )
            + y_tour[tour]
            * (
                mdl.sum(
                    tour.allocation_at_poi[poi][alloc] * rc_conectivity[(tour, poi)]
                    for poi, alloc in [
                        (poi, alloc) for poi in POI_LIST for alloc in range(1, 481)
                    ]
                )
            )
            for tour in all_tours
        )
    )

    # Constraints

    # For each user, one tour must be chosen
    for user in users:
        mdl.add_constraint(mdl.sum(y_tour[tour] for tour in user.tours) == 1)

    # If a tour is chosen, allocate a eccru for each visit in it
    for tour in all_tours:
        for poi in POI_LIST:
            mdl.add_constraint(
                mdl.sum(a_eccru[(tour, poi, eccru)] for eccru in eccrus)
                == y_tour[tour] * tour.visits[poi]
            )

    for tour in all_tours:
        for poi in POI_LIST:
            mdl.add_constraint(
                rc_conectivity[(tour, poi)]
                <= (tour.max_bandwitdh * y_tour[tour] * tour.visits[poi])
            )
            mdl.add_constraint(
                rc_conectivity[(tour, poi)]
                >= (tour.min_bandwidth * y_tour[tour] * tour.visits[poi])
            )
            for eccru in eccrus:
                mdl.add_constraint(
                    rc_computational[(tour, poi, eccru)]
                    <= (
                        tour.max_computational
                        * a_eccru[(tour, poi, eccru)]
                        * tour.visits[poi]
                    )
                )
                mdl.add_constraint(
                    rc_computational[(tour, poi, eccru)]
                    >= (
                        tour.min_computational
                        * a_eccru[(tour, poi, eccru)]
                        * tour.visits[poi]
                    )
                )

    # Computational resource allocation
    for eccru in eccrus:
        for alloc in range(1, 481):
            mdl.add_constraint(
                mdl.sum(
                    tour.allocation_at_poi[poi][alloc]
                    * rc_computational[(tour, poi, eccru)]
                    for tour, poi in [
                        (tour, poi) for tour in all_tours for poi in POI_LIST
                    ]
                )
                <= eccru.capacity
            )

    # Connectivity resource allocation
    for poi in POI_LIST:
        for alloc in range(1, 481):
            mdl.add_constraint(
                mdl.sum(
                    tour.allocation_at_poi[poi][alloc] * rc_conectivity[(tour, poi)]
                    for tour in all_tours
                )
                <= net_cap
            )

    mdl.print_information()
    #mdl.export_as_lp(path="./")

    global smarttour_runtime
    if mdl.solve():
        smarttour_runtime = mdl.solve_details.time
        mdl.print_solution()
        mdl.report()
        # Save the CPLEX solution as "solution.json" program output
        #with get_environment().get_output_stream("solution_plusTour.json") as fp:
        #    mdl.solution.export(fp, "json", keep_zeros=True)

        for pair in mdl.solution.iter_var_values():
            if "rc_computational" in pair[0].name:
                splits = pair[0].name.split("_")
                tour_id = int(splits[2])
                poi_id = int(splits[3])
                eccru_id = int(splits[4])

                all_tours[tour_id].computational_allocation[poi_id] = (
                    eccru_id,
                    pair[1],
                )

            if "rc_conectivity" in pair[0].name:
                splits = pair[0].name.split("_")
                tour_id = int(splits[2])
                poi_id = int(splits[3])

                all_tours[tour_id].bandwidth_allocation[poi_id] = pair[1]

        for pair in mdl.solution.iter_var_values():
            if "y_tour" in pair[0].name and round(pair[0].solution_value) == 1:
                # print "Printing information for tour {}".format(pair[0].name[7:])
                # tour_index = tnt(pair[0].name[7:])
                chosen_tours.append(all_tours[int(pair[0].name[7:])])
                # print all_tours[int(pair[0].name[7:])].path
                # print all_tours[int(pair[0].name[7:])].allocation

        # json_out_file = open("results.json", "w")
        # json.dump(chosen_tours, json_out_file, default=serialize, indent=2)
        # json_out_file.close()

        # for pair in mdl.solution.iter_var_values():
        # 	if 'y_tour' in pair[0].name:
        # 		print "Printing information for tour {}".format(pair[0].name[7:])
        # 		print all_tours[int(pair[0].name[7:])].path
        # 		print all_tours[int(pair[0].name[7:])].allocation

        # 		for visit in all_tours[int(pair[0].name[7:])].path:
        # 			print all_tours[int(pair[0].name[7:])].visits[visit]
        # 			print all_tours[int(pair[0].name[7:])].allocation_at_poi[visit]
        # 			print "Allocated for {} with amount {}: ECU {}".format(visit, allocated_for_tour[int(pair[0].name[7:])][int(visit)][1], allocated_for_tour[int(pair[0].name[7:])][int(visit)][0])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--city", help="Insert City name or the string 'all' to run for all cities.")
    parser.add_argument("-i", "--instance", help="Insert the model instance in the options high/medium/low.")
    args = parser.parse_args()

    instance = args.instance
    if instance == 'high':
        net_cap = 75
    elif instance == 'medium':
        net_cap = 150
    elif instance == 'low':
        net_cap = 300

    print(instance)


    RUNNING_DIR = os.path.dirname(os.path.abspath(__file__))


    if args.city != 'all':
        cities = [args.city]
        print(cities) 
    else:
        cities = read_cities_to_solve("file_of_cities.in")

    all_tours = []
    users = []
    eccrus = []
    profiles = []
    chosen_tours = []
    optimal_tours = []

    smarttour_runtime = 0

    profile1 = AppProfile(1)
    profile2 = AppProfile(2)
    profile3 = AppProfile(3)
    profile4 = AppProfile(4)

    profile1.add_application(Application("AR", 1, 10, 0.1, 1))
    profile1.add_application(Application("Video Streaming", 1.5, 25, 0, 0))

    profile2.add_application(Application("AR", 1, 10, 0.1, 1))

    profile3.add_application(Application("Video Streaming", 1.5, 25, 0, 0))

    profiles.append(profile1)
    profiles.append(profile2)
    profiles.append(profile3)
    profiles.append(profile4)

    num_sols = 0

    eccrus.append(ECCRU(1, 37.5))
    eccrus.append(ECCRU(2, 37.5))

    for city in cities:
        os.chdir("{}/../Datasets/Model/{}".format(RUNNING_DIR, city))
        all_tours = []
        users = []
        chosen_tours = []
        optimal_tours = []
        num_sols = 0
        smarttour_runtime = 0

        poi_list("pois.in")
        
        process_solutions("../../../plusTour/results/{}/tour_file.in".format(city))
        build_allocation_model()

        sol_output = {
            "plusTour_tours": chosen_tours,
            "plusTour_runtime": smarttour_runtime,
        }

        json_out_file = open("../../../plusTour/results/{}/results.json".format(city), "w")
        json.dump(sol_output, json_out_file, default=serialize, indent=2)
        json_out_file.close()
