CC = g++
CFLAGS  = -std=c++17 -g -fPIC
INCLUDES = -I./include 

all:	tourist
		@echo Compiling Tourist
		-rm *.o
		@echo Finished

tourist:	main.o City.o CityContainer.o POI.o POIContainer.o Tour.o TourContainer.o User.o UserContainer.o Visit.o SPPRCSolver.o SPPRCResourceSolver.o MultiTourSolver.o ECCRU.o ECCRUContainer.o Service.o
					$(CC) $(CFLAGS) $(INCLUDES) main.o City.o CityContainer.o POI.o POIContainer.o Tour.o TourContainer.o User.o UserContainer.o Visit.o SPPRCSolver.o SPPRCResourceSolver.o MultiTourSolver.o Service.o ECCRU.o ECCRUContainer.o -o tourist

main.o: src/main.cpp POI.o POIContainer.o
					$(CC) $(CFLAGS) $(INCLUDES) -c src/main.cpp

City.o: src/tourist/City.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/City.cpp

CityContainer.o: src/tourist/CityContainer.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/CityContainer.cpp

POI.o: src/tourist/POI.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/POI.cpp

POIContainer.o: src/tourist/POIContainer.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/POIContainer.cpp

Tour.o: src/tourist/Tour.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/Tour.cpp

TourContainer.o: src/tourist/TourContainer.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/TourContainer.cpp

User.o: src/tourist/User.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/User.cpp

UserContainer.o: src/tourist/UserContainer.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/UserContainer.cpp

Visit.o: src/tourist/Visit.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/Visit.cpp

SPPRCSolver.o: src/tourist/SPPRCSolver.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/SPPRCSolver.cpp

SPPRCResourceSolver.o: src/tourist/SPPRCResourceSolver.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/SPPRCResourceSolver.cpp

MultiTourSolver.o: src/tourist/MultiTourSolver.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/MultiTourSolver.cpp

ECCRU.o: src/tourist/ECCRU.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/ECCRU.cpp

ECCRUContainer.o: src/tourist/ECCRUContainer.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/ECCRUContainer.cpp

Service.o: src/tourist/Service.cpp
					$(CC) $(CFLAGS) $(INCLUDES) -c src/tourist/Service.cpp