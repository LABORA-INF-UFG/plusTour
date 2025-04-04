import os
import json
import csv
import argparse


mec_cap = 75
net_cap = 300


def POIs_visits(data_city):
    visits = {}
    list_visits = []
    
    dict_POI, list_POI = dictionary_of_POIs_ids()
    
    for item in data_city:
        for seq in item['generated']:
            visits[seq] = visits.get(seq, 0) + 1
   
    for item in list_POI:
        if visits.get(int(item), None) == None:
            visits[item] = 0

    for item in visits:
        visit_dict = {'POI':item, 'visits':visits[item]}

        list_visits.append(visit_dict)
    
    list_visits.sort(key= lambda x: (x['visits'] is not None, x['visits']), reverse=True)
    return list_visits
            
    

def number_of_POIs():
    dict_POI, list_POI = dictionary_of_POIs_ids()
    return len(list_POI)

def generate_data_for_intersection_of_N_users(data_list, N=20):
    n_users = []

    for item in data_list:
        if item['AE'] is not None and item['UE'] is not None:
            n_users.append(item)
    
    n_users.sort(key= lambda x: (x['AE'] is not None, x['AE']), reverse=True)
    top_users_AE = list(n_users)
    top_users_AE = top_users_AE[0:N]
    
    n_users.sort(key= lambda x: (x['AE'] is not None, x['AE']), reverse=False)
    bot_users_AE = list(n_users)
    bot_users_AE = bot_users_AE[0:N]
    
    n_users.sort(key= lambda x: (x['UE'] is not None, x['UE']), reverse=True)
    top_users_UE = list(n_users)
    top_users_UE = top_users_UE[0:N]
    
    n_users.sort(key= lambda x: (x['UE'] is not None, x['UE']), reverse=False)
    bot_users_UE = list(n_users)
    bot_users_UE = bot_users_UE[0:N]
    
    return top_users_AE, top_users_UE, bot_users_AE, bot_users_UE
    

def dictionary_of_POIs_ids():
    dict_of_poi_id = {}
    list_of_poi_id = []
    
    with open('pois.in') as file:
        for line in file:
            splitted_line = line.rstrip().split(' ')
            dict_of_poi_id[splitted_line[0]] = [0]*481
            list_of_poi_id.append(splitted_line[0])

    return dict_of_poi_id, list_of_poi_id
    
def bandwidth_usage(model_, city):
    dict_of_poi_id, list_of_poi_id = dictionary_of_POIs_ids()

    with open('../../../plusTour/results/{}/results.json'.format(city)) as file:
        data = json.load(file)
        tours = data[model_+'_tours']
        
    for i in range(1, 481):
        for user_sol in tours:
            for seq in user_sol['bandwidth_allocation']:
                dict_of_poi_id[seq][i] = dict_of_poi_id[seq][i] + user_sol['allocation_at_poi'][seq][i]*user_sol['bandwidth_allocation'][seq]

        for item in list_of_poi_id:
            dict_of_poi_id[item][i] = float("%.2f" % (dict_of_poi_id[item][i]/net_cap))

    return dict_of_poi_id              

def mec_s_usage(model_, city):
    alloc_in_t = [0]*481

    with open('../../../plusTour/results/{}/results.json'.format(city)) as file:
        data = json.load(file)
        tours = data[model_+'_tours']
        
    for i in range(1, 481):
        for user_sol in tours:
            for seq in user_sol['computational_allocation']:
                alloc_in_t[i] = alloc_in_t[i] + user_sol['allocation_at_poi'][seq][i]*user_sol['computational_allocation'][seq][1]
                
        alloc_in_t[i] = float("%.2f" % (alloc_in_t[i]/(mec_cap)))
                    
    return alloc_in_t
    
def normalize(value, max_profit, min_profit):
    return (value - min_profit)/(max_profit-min_profit)

def num_of_tours_csv(number_tours, total_tours, cities):
    os.chdir("{}".format(RUNNING_DIR))
    
    data = []
    header = cities
    
    with open('results/first_phase_num_tours' + '.csv', 'w', encoding='UTF8') as file:

        writer = csv.writer(file)
        
        writer.writerow(header)

        for item in range(1, 251):
            data = []
            for city in cities:
                data.append(number_tours[city][item])

            writer.writerow(data)
            
        data = []
        for city in cities:
            data.append(total_tours[city])

        writer.writerow(data)
    
def sequence(model_, city):
    seq_dict = {}
    data_list = []
      
    with open('../../../plusTour/results/{}/results.json'.format(city)) as file:
        data = json.load(file)
        tours = data[model_ + '_tours']
        
        min_profit = None
        max_profit = None
        number_of_users_with_service_demand = 0
        for i in tours:
            if i['app_profile']['id'] != 4:
                if min_profit == None or i['profit'] < min_profit:
                    min_profit = i['profit']
                if max_profit == None or i['profit'] > max_profit:
                    max_profit = i['profit']
                    
                number_of_users_with_service_demand = number_of_users_with_service_demand + 1
            
        for i in tours:
            data_dict = {}
            if i['app_profile']['id'] != 4:
                sum_bandwidth = 0
                sum_comput_rc = 0
                for item in i['visits']:
                    if i['visits'][item] != 0:
                        if i['bandwidth_allocation'].get(str(item), None) != None:
                            sum_bandwidth = sum_bandwidth + (i['bandwidth_allocation'][str(item)]/(2*i['num_visits']*i['max_bandwitdh']))
                        
                        if i['app_profile']['id'] == 3:
                            sum_comput_rc = 0.5
                        elif i['computational_allocation'].get(str(item), None) != None:
                            mec_s, alloc = i['computational_allocation'][str(item)]
                            sum_comput_rc = sum_comput_rc + (alloc/(2*i['num_visits']*i['max_computational']))
            
            
            data_dict['user_code'] = i['user_code']
            data_dict['user'] = i['user']
            data_dict['app_profile'] = i['app_profile']['id']
            data_dict['seq_id']  = i['seq_id']
            data_dict['profit_n']  = normalize(i['profit'], max_profit, min_profit)
            data_dict['real_profit']  = i['profit']
            data_dict['original']  = i['original_seq']
            data_dict['generated']  = i['path']
            data_dict['Recall'] = len(set(i['original_seq']) & set(i['path']))/len(i['original_seq'])
            data_dict['Precision'] = len(set(i['original_seq']) & set(i['path']))/len(i['path'])
            if data_dict['Recall'] == 0 and data_dict['Precision'] == 0:
                data_dict['F-Score'] = 0
            else:
                data_dict['F-Score'] = (2*data_dict['Precision']*data_dict['Recall'])/(data_dict['Precision']+data_dict['Recall'])

            if i['app_profile']['id'] != 4:
                data_dict['AE'] = sum_bandwidth + sum_comput_rc
                data_dict['UE'] = data_dict['AE']*data_dict['F-Score']
            else:
                data_dict['AE'] = None
                data_dict['UE'] = None
                
            data_list.append(data_dict)

    top_users_for_AE, top_users_for_UE, bot_users_for_AE, bot_users_for_UE, = generate_data_for_intersection_of_N_users(data_list)
    
    return data_list, number_of_users_with_service_demand, top_users_for_AE, top_users_for_UE, bot_users_for_AE, bot_users_for_UE
    
def num_of_tours(city):
    json_data = {}
    cont = 1
    file = open('../../../plusTour/results/{}/plot_num_of_tours_file.in'.format(city), "r")
    data = file.readlines()
    num_users = int(data[0].rstrip())
    
    for i in range(1, num_users + 1):
        
        json_data[cont] = int(data[i].rstrip().split(' ')[1])
        cont = cont + 1
        
    total_tours = int(data[num_users + 1].rstrip())
        
    return (json_data, total_tours)
        
    
def time_of_fases(model_, city):
    fase_1_time = 0
    fase_2_time = 0
    total_time = 0
    with open('../../../plusTour/results/{}/plot_time_fase_1_file.in'.format(city)) as file:
        for line in file:
            fase_1_time = line.rstrip()
            fase_1_time = float(fase_1_time)
    
    with open('../../../plusTour/results/{}/results.json'.format(city)) as file:
        data = json.load(file)
        fase_2_time = float(data[model_+'_runtime'])
        
    total_time = fase_1_time + fase_2_time
    
    return {'fase_1_time': fase_1_time, 'fase_2_time': fase_2_time, 'total_time': total_time}
        

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--cities", help="Insert City1/City2/CityN names or the string 'all' to run for all cities.")
    parser.add_argument("-i", "--instance", help="Insert the model instance in the options high/medium/low.")
    args = parser.parse_args()

    results_dir = 'results'
    os.makedirs(results_dir, exist_ok=True)

    data = {}
    RUNNING_DIR = os.path.dirname(os.path.abspath(__file__))



    if args.cities != 'all':
        cities = args.cities.split('/')

    else:
        cities = ['Athens', 'Barcelona', 'Budapest', 'Edinburgh', 'Glasgow', 'London',
                'Madrid', 'Melbourne', 'NewDelhi', 'Osaka', 'Perth', 'Toronto', 'Vienna']
        
    print(cities)

    instance = args.instance
    if instance == 'high':
        net_cap = 75
    elif instance == 'medium':
        net_cap = 150
    elif instance == 'low':
        net_cap = 300

    print(instance)
    
    number_of_tours = {}
    total_tours = {}
    
    for model_ in ['plusTour']:
        results_ = 'results/' + model_ + '_' + instance + '.json'
        for city in cities:
            data[city] = {}

            os.chdir("{}/../Datasets/Model/{}".format(RUNNING_DIR, city))

            data[city]['time'] = time_of_fases(model_, city)

            number_of_tours[city], total_tours[city] = num_of_tours(city)
            
            (data[city]['sequence_results'], data[city]['num_users_with_service_demand'], 
             data[city]['top_users_AE'], data[city]['top_users_UE'],
             data[city]['bot_users_AE'], data[city]['bot_users_UE']) = sequence(model_, city)
            
            data[city]['mec_s_usage_at_t'] = mec_s_usage(model_, city)

            data[city]['bandwidth_usage_at_t'] = bandwidth_usage(model_, city)

            data[city]['number_of_POIs'] = number_of_POIs()
            data[city]['POIs_visits'] = POIs_visits(data[city]['sequence_results'])
        
        num_of_tours_csv(number_of_tours, total_tours, cities)
        
        os.chdir("{}".format(RUNNING_DIR))
        with open(results_, 'w') as json_file:
            json.dump(data, json_file, indent=4)