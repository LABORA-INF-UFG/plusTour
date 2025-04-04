# +Tour: Recommending personalized itineraries for smart tourism

+Tour was tested in Debian “bullseye” 11. Although it may work on some other Debian-based Linux distributions, we do not guarantee that all features will work well.

- [Getting started](#getting-started)
	- [Installing the prerequisites](#installing-the-prerequisites)
	- [Cloning the repository](#cloning-the-repository)
- [Data analysis](#data-analysis)
- [Datasets](#datasets)
- [+Tour](#tour)
	- [Phase 1](#phase-1)
	- [Phase 2](#phase-2)
	- [Processing the results](#processing-the-results)
- [+Tour Elsevier Computer Networks](#tour-elsevier-computer-networks)
	- [Citation](#citation)
- [Contact us](#contact-us)

## Getting started

These instructions will guide you to get the environment up and running.

### Installing the prerequisites

```
sudo apt update
sudo apt install python3 python3-pip git libboost-all-dev
pip3 install --upgrade docplex
```

**It is also necessary to install the [IBM ILOG CPLEX Optimization Studio](https://www.ibm.com/br-pt/products/ilog-cplex-optimization-studio):**

1. Download the binary on the [project's website](https://www.ibm.com/br-pt/products/ilog-cplex-optimization-studio);

2. Run the following command in the directory where the file is located, replacing `<CPLEX_BINARY_FILE.bin>` with the correct filename:

```
bash <CPLEX_BINARY_FILE.bin>
```

3. Install CPLEX at the following directory, replacing `<USER>` with your username:

```
/home/<USER>/CPLEX_Studio221
```

4. After the installation, configure the python API with:

```
sudo python3 /home/$USER/CPLEX_Studio221/python/setup.py install
```

5. Insert the path where CPLEX is installed in the `~/.bashrc` file with:

```
export PATH=$PATH:/home/$USER/CPLEX_Studio221/cplex/bin/x86-64_linux
```

6. Finally, execute:

```
source .bashrc
```

### Cloning the repository

```
git clone https://github.com/LABORA-INF-UFG/plusTour.git
```

## Data analysis

The complete data analysis is available at [Analysis/](Analysis/), where eahc directory is named after each corresponding figure in the paper.

## Datasets

The data sets used in our work can be found at [Datasets/](Datasets/), where:

- **[Datasets/Raw/](Datasets/Raw/):** contains the raw data extracted from [Flickr](https://www.flickr.com/services/developer/api/);
- **[Datasets/Model/](Datasets/Model/):** contains the data processed for the model (+Tour);
- **[Datasets/Analysis/](Datasets/Analysis/):** contains the data processed for the data analysis.

The datasets used for the data analysis are organized as follows:

- **seqID:** an identifier for the sequence;
- **shortSeq:** a boolean that identifies if the sequence is short (True) or not (False). A short sequence is a sequence in which the user visited less than 3 different POIs (Points of Interest);
- **startTime:** the date when the sequence started, i.e., when the user took the first photo;
- **endTime:** the date when the sequence ended, i.e., when the user took the last photo;
- **seqDuration:** the sequence duration in seconds;
- **numberOfPhotos:** the number of photos taken by the user in the sequence;
- **numberOfDifferentPOIs:** the number of different POIs the user visited in the sequence;
- **photoID:** an identifier for each photo;
- **poiID:** an identifier for each POI;
- **poiName:** the POI name;
- **poiTheme:** the theme of the POI;
- **poiLat:** the POI latitude;
- **poiLon:** the POI longitude;
- **dateTaken:** the date when the photo was taken;
- **unixTimestamp:** the date when the photo was taken in Unix timestamp;
- **userID:** an identifier for the user.

## +Tour

Here we detail how to run +Tour:

### Phase 1

This phase is responsible for generating itineraries. It is written in C++, and is located at **[plusTour/espprc](plusTour/espprc)**. You can compile the model with:

```
make -C plusTour/espprc/
```

Then, to run Phase 1 of +Tour, enter the directory:

```
cd plusTour
```

And execute:

```
./espprc/tourist -solve α type city
```

Where:
* **α** – Represents the weight given to the user’s interest or the POI popularity (α).
* **type** – Represents the type of user interest. The available values are:
  * **1:** frequency-based interest;
  * **2:** time-based interest;
  * **3:** time-based interest (weighted).
* **city** – Represents the city considered in the model. The available cities are: `Athens`, `Barcelona`, `Budapest`, `Edinburgh`, `Glasgow`, `London`, `Madrid`, `Melbourne`, `NewDelhi`, `Osaka`, `Perth`, `Toronto`, and `Vienna`. To solve for all cities, use `all`.

Here is an example:

```
./espprc/tourist -solve 0.5 2 Athens
```

After the execution, three files will be generated in a directory called `results`: `plot_time_fase_1_file.in`, `plot_num_of_tours_file.in` and `tour_file.in`. Where:

* **plot_time_fase_1_file.in:** Contains the execution time of Phase 1 in seconds;
* **plot_num_of_tours_file.in:** Contains the number of tours generated for each user;
* **tour_file.in:** Contains the specifications of the tours generated, where the first line represents the number of users, and the rest containing the tours per user in the following format:
	* **User ID:** identity of a user;
  	* **Number of tours:** number of recommended tours generated for that user;
  	* **Tours:** A list of tours with `Number of tours` items. For each entry in `Tours`, we have the following structure representing the tour characterization:
  		* **Sequence ID and original sequence:** Identification of the original sequence and the POIs visitation in that sequence respectivelly;
  		* **Profit:** The profit perceived in that tour;
  		* **Tour path:** The sequence of POI visitation on the tour;
  		* **Time allocation:** A 480 item list representing each minute in 8 hours, where for each item, we have 0 if in that minute the user is not consuming resources when using the application and 1 otherwise;
  		* **Number of POIs:** Number of POIs available in that city;
  		* **Time allocation per POI:** The same as `Time allocation` but in this case we have a line for each POI, hence mapping the resource usage in each one of the POIs;

### Phase 2

This phase is responsible for allocating resources, it is written in Python and it is represented by the file `plusTour.py`, located at **[plusTour/](plusTour/)** directory.

To run Phase 2 of +Tour (assuming you are already inside the **[plusTour/](plusTour/)** directory):

```
python3 plusTour.py --city City --instance instance
```

Where:
* **city** – Represents the city considered in the model. The available cities are: `Athens`, `Barcelona`, `Budapest`, `Edinburgh`, `Glasgow`, `London`, `Madrid`, `Melbourne`, `NewDelhi`, `Osaka`, `Perth`, `Toronto`, and `Vienna`. To solve for all cities, use `all`.
* **instance** – Represents the network overload. The available values are:
  * **high** overload;
  * **medium** overload;
  * **low** overload.

Here is an example:

```
python3 plusTour.py --city Athens --instance high
```

After the execution, a `results.json` file will be generated in `results`. In the following format:

* **User** – Number of sequence of the user;
* **User code** – Same as `User ID`;
* **ID** – Second number of sequence of the user;
* **Allocation** – Same as `Time allocation`;
* **Allocation at POI** – Same as `Time allocation per POI`;
* **Profit** – Same as `Profit` (in `tour_file.in`);
* **Path** – Same as `Tour path`;
* **Original sequence** – Same as `original sequence`;
* **APP profile** – The profile of the aplication used by the user, with the following structure:
	* **Name** – Name of the application;
	* **APP min bandwidth** – Minimum value of bandwidth required by the application;
	* **APP max bandwidth** – Maximum value of bandwidth required by the application;
	* **APP min computational** – Minimum value of computational resources required by the application;
	* **APP max computational** – Maximum value of computational resources required by the application.
* **Min bandwidth** – Minimum value of bandwidth required between all user applications;
* **Max bandwidth** – Maximum value of bandwidth required between all user applications;
* **Min computational** – Minimum value of computational resources required between all user applications;
* **Max computational** – Maximum value of computational resources required between all user applications;
* **Total bandwidth alloc** – Total value of bandwidth required between all user applications during all the time (8 hours);
* **Total computational alloc** – Total value of computational resources required between all user applications during all the time (8 hours);
* **Visited** – POI visitation of user for each minute;
* **Visits** – The POI visitation based on the tour;
* **Num visits** – Length of the tour;
* **Bandwidth allocation** – Bandwidth allocated for each POI visit;
* **Computational allocation** – Computational allocation for each POI visit;
* **Sequence ID** – Same as `Sequence ID` (in `tour_file.in`).

### Processing the results

For your convenience, we created a script to process the results, which you can run with the following command (assuming you are already inside the **[plusTour/](plusTour/)** directory):

```
python3 generate_json_for_plot.py --cities City --instance instance
```

Where:
* **city** – Represents the city considered in the model. The available cities are: `Athens`, `Barcelona`, `Budapest`, `Edinburgh`, `Glasgow`, `London`, `Madrid`, `Melbourne`, `NewDelhi`, `Osaka`, `Perth`, `Toronto`, and `Vienna`. To solve for all cities, use `all`.
* **instance** – Represents the network overload. The available values are:
  * **high** overload;
  * **medium** overload;
  * **low** overload.

Here is an example:

```
python3 generate_json_for_plot.py --cities Athens --instance high
```

After the execution, a `first_phase_num_tours.csv` file will be generated in `results`, containing the number of tours generated in Phase 1 per user and total (last line). A second file `plusTour_<instance>.json` contains the results of both Phases for each city in the following structure:

* **Time** – Represents the execution time for all Phases and total, with the following structure:
  * **Phase 1 time:** Execution time of Phase 1;
  * **Phase 2 time:** Execution time of Phase 2;
  * **Total time:** Execution time total.
* **Sequence results** – Tour recommended to a user, with the following structure:
  * **User code:** Same as `User ID`;
  * **User:** Same as `User` (in `results.json`);
  * **App profile:** The profile of APP utilization of the user;
  * **Seq ID:** Same as `Sequence ID`;
  * **Profit n:** Normalized profit;
  * **Real profit:** Same as `Profit`;
  * **Original:** Same as `original sequence`;
  * **Generated:** Same as `Tour path`;
  * **Recall:** Value of the **Recall** metric;
  * **Precision:** Value of the **Precision** metric;
  * **F-Score:** Value of the **F-Score** metric;
  * **AE:** Value of the **Allocation Efficience** metric;
  * **UE:** Value of the **User Experience** metric.

## +Tour Elsevier Computer Networks

For more information, read the [+Tour Elsevier Computer Networks Paper](https://arxiv.org/abs/2502.17345).

### Citation

Feel free to use the data sets and +Tour! Please do not forget to cite our paper! :)

```
@article{ESPER2025111118,
	title = {{+Tour: Recommending personalized itineraries for smart tourism}},
	journal = {Computer Networks},
	volume = {260},
	pages = {111118},
	year = {2025},
	issn = {1389-1286},
	doi = {https://doi.org/10.1016/j.comnet.2025.111118},
	url = {https://www.sciencedirect.com/science/article/pii/S1389128625000866},
	author = {João Paulo Esper and Luciano de S. Fraga and Aline C. Viana and Kleber Vieira Cardoso and Sand Luz Correa},
	keywords = {Travel itinerary recommendation, Next-generation touristic services, Multi-access edge computing, Advanced mobile networks}
}
```

## Contact us

If you would like to contact us to contribute to this project, ask questions or suggest improvements, feel free to e-mail us at: joaopauloesper@gmail.com and lucianosouza@discente.ufg.br.