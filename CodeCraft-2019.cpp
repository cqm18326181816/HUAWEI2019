#include "reading.h"
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include "external.h"
#define MAX_VALUE 10000
//#define MAX_VERTEX_COUNT 36
#define Infinity 0xffff

using namespace std;
int n_period;
int num_each_period;
int cars_num;
int roads_num;
int bias[100][100];
int B[100][100];
int w;
int v;
car cars_array[20000];
vector<cross> crosses;
vector<road> roads;
vector<car> cars;
ofstream out;
struct MGraph
{
	int *edges[MAX_VALUE];
	int iVertexCount, iEdageCount;
};
void ReadData(MGraph *mGraph);
void Floyd(MGraph *mGraph, int *iArrPath[MAX_VALUE]);
void obtain_roadcondition(MGraph *mGraph, int *iArrPath[MAX_VALUE]);
void PrintResult(MGraph *mGraph, int *iArrPath[MAX_VALUE], string answer_path);
void Printbais(void);
reading::reading()
{
//    //data from files
//    car_data = "car.txt";
//    cross_data = "cross.txt";
//    road_data = "road.txt";

}

reading::~reading()
{
}

void reading::read_from_files()
{
    fstream fsl(car_data, ios::in);
    string line;
    while (getline(fsl, line))
    {
        char ch;
        istringstream ss(line);
        ss >> ch;
        if (ch == '#')continue;
        else if (ch != '(')error("bad begin!");
        int id, from, to, speed, planTime;float weight = 0.0;
        ss >> id >> ch >> from >> ch >> to >> ch >> speed >> ch >> planTime >> ch;
        if (ch != ')')error(" 'car.txt' bad ending!");
        cars.push_back(car{ id, from, to, speed, planTime });
        ss.clear();
    }
    fstream fs2(cross_data, ios::in);
    while (getline(fs2, line))
    {
        char ch = ' ';
        istringstream ss(line);
        ss >> ch;
        if (ch == '#')continue;
        else if (ch != '(')error("bad begin!");
        int id = 0, roadIdN = 0, roadIdS = 0, roadIdW = 0, roadIdE = 0;
        ss >> id >> ch >> roadIdN >> ch >> roadIdS >> ch >> roadIdW >> ch >> roadIdE >> ch;
        if (ch != ')')error("'cross.txt' bad ending!");
        crosses.push_back(cross{ id, roadIdN, roadIdS, roadIdW, roadIdE });

    }

    fstream fs3(road_data, ios::in);
    while (getline(fs3, line))
    {
        char ch;
        istringstream ss(line);
        ss >> ch;
        if (ch == '#')continue;
        else if (ch != '(')error("bad begin!");
        int id = 0, length = 0, speed = 0, channel = 0, from = 0, to = 0, isDuplex = 0;
        ss >> id >> ch >> length >> ch >> speed >> ch >> channel >> ch >> from >> ch >> to >> ch >> isDuplex >> ch;
        if (ch != ')')error("'road.txt' bad ending!");
        roads.push_back(road{ id, length, speed, channel, from, to, isDuplex });
    }

    int max_carspeed = cars[0].speed;
    int min_carspeed = cars[0].speed;
    int max_carsplantime = cars[0].planTime;
    int min_carsplantime = cars[0].planTime;
    for (int x = 0; x < cars.size(); x++){

            if (cars[x].speed > max_carspeed)max_carspeed = cars[x].speed;
            if (cars[x].speed < min_carspeed)min_carspeed = cars[x].speed;
            if (cars[x].planTime > max_carsplantime)max_carsplantime = cars[x].planTime;
            if (cars[x].planTime < min_carsplantime)min_carsplantime = cars[x].planTime;
        }
        int max_roadspeed = roads[0].speed;
        int min_roadspeed = roads[0].speed;
        int max_roadschannel = roads[0].channel;
        int min_roadschannel = roads[0].channel;
        int max_roadslength = roads[0].length;
        int min_roadslength = roads[0].length;
        for (int x = 0; x < roads.size(); x++){

            if (roads[x].speed > max_roadspeed)max_roadspeed = roads[x].speed;
            if (roads[x].speed < min_roadspeed)min_roadspeed = roads[x].speed;
            if (roads[x].channel > max_roadschannel) max_roadschannel = roads[x].channel;
            if (roads[x].channel < min_roadschannel) min_roadschannel = roads[x].channel;
            if (roads[x].length > max_roadslength)max_roadslength = roads[x].length;
            if (roads[x].length < min_roadslength)min_roadslength = roads[x].length;

        }
        for (int x = 0; x < cars.size(); x++){
                cars[x].weight =100*((cars[x].speed - min_carspeed) /(1.0*(max_carspeed - min_carspeed)) - (cars[x].planTime - min_carsplantime) /(1.0*(max_carsplantime - min_carsplantime)));
                cars_array[x] = cars[x];
            }
            for (int i = 1; i < cars.size(); i++){
                car key = cars_array[i];
                int j = i - 1;
                while ((j >= 0) && (key.weight > cars_array[j].weight)){
                    cars_array[j + 1] = cars_array[j];
                    j--;
                }
                cars_array[j + 1] = key;
            }

        cars_num = cars.size();
        roads_num = roads.size();
        int	length_sum=0;
        for (int i = 0; i < roads_num; i++){
            length_sum = length_sum + (roads[i].channel)*(roads[i].length);  //总容量
        }
        
        num_each_period = length_sum /(3.5*(max_roadspeed + min_roadspeed));
        int carswaittime_ave;
        carswaittime_ave = (length_sum / roads_num) / min_roadspeed;
		
        
        n_period = cars_num /num_each_period;
		cout << n_period << " " << length_sum << " " << num_each_period << " " << carswaittime_ave << " " << endl;
        for (int i = 0; i < n_period; i++){
            for (int j = 0; j < cars_num; j++){
                if (j >(cars_num - i*num_each_period + 1)){
                cars_array[j].planTime = cars_array[j].planTime + carswaittime_ave;
				}
			}
        }


    for (int i = 1; i <= crosses.size(); i++)
    {
        for (int j = 1; j <= crosses.size(); j++)
        {
            B[i][j] = 999999;
            bias[i][j] = 0;
        }
    }
    for (int i = 0; i < roads.size(); i++){
            int  b1 = roads[i].from;
            int  c1 = roads[i].to;
            if (roads[i].isDuplex){
                B[b1][c1] = roads[i].length/2;
                B[c1][b1] = roads[i].length/2;
            }
            else B[b1][c1] = roads[i].length/2;
        }
 }


int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
    if(argc < 5){
        std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
        exit(1);
    }
    std::string carPath(argv[1]);
    std::string roadPath(argv[2]);
    std::string crossPath(argv[3]);
    std::string answerPath(argv[4]);
    std::cout << "carPath is " << carPath << std::endl;
    std::cout << "roadPath is " << roadPath << std::endl;
    std::cout << "crossPath is " << crossPath << std::endl;
    std::cout << "answerPath is " << answerPath << std::endl;

    // TODO:read input filebuf
    reading rd;
    rd.car_data =carPath;
    rd.cross_data =crossPath ;
    rd.road_data =roadPath ;
    rd.answer_data=answerPath;
	out.open(answerPath, ios::out);
	out << "#(carID,StartTime,RoadID...)" << endl;
    rd.read_from_files();
    int *iArrPath[MAX_VALUE];
    for (int i = 0; i < MAX_VALUE; i++){
        iArrPath[i] = new int[MAX_VALUE];
    }
    MGraph mGraph;
    for (int i = 0; i < MAX_VALUE; i++){
        mGraph.edges[i] = new int[MAX_VALUE];
    }
	// TODO:process

	for (w = 0; w <= n_period+1; w++){
		for (int x = 1; x <= crosses.size(); ++x){                                                  //bias初始化
			for (int y = 1; y <= crosses.size(); ++y){
				bias[x][y] = 0;
			}
		}
		for (int x = 0; x < 7; x++){
				ReadData(&mGraph);
				Floyd(&mGraph, iArrPath);
				obtain_roadcondition(&mGraph, iArrPath);
				Printbais();
				//
				}

				// TODO:write output file
		PrintResult(&mGraph, iArrPath, rd.answer_data);
			//}
		cout << "@@@@@@@@" <<" "<< w<<endl;
	}
	out.close();
	system("pause");
    return 0;
}
void ReadData(MGraph *mGraph)
{
    int iRow , iCol ;

    mGraph->iVertexCount = crosses.size();
    iRow = mGraph->iVertexCount;
    iCol = mGraph->iVertexCount;
    for (iRow = 1; iRow <=crosses.size(); iRow++){
        for (iCol = 1; iCol <=crosses.size(); iCol++){
            B[iRow][iCol] = B[iRow][iCol]/2+bias[iRow][iCol]*50;              //更新邻接矩阵
            mGraph->edges[iRow][iCol] = B[iRow][iCol];
           //cout << mGraph->edges[iRow][iCol]<<" ";
        }
    }
}

void Floyd(MGraph *mGraph, int **iArrPath){

    for (int i = 1; i <= mGraph->iVertexCount; i++){
        for (int j = 1; j <= mGraph->iVertexCount; j++){
            iArrPath[i][j] = i;
        }
    }//初始化路径表

    for (int k = 1; k <= mGraph->iVertexCount; k++){
        for (int i = 1; i <= mGraph->iVertexCount; i++){
            for (int j = 1; j <= mGraph->iVertexCount; j++)
            {
                if (mGraph->edges[i][k] + mGraph->edges[k][j] < mGraph->edges[i][j]){
                    mGraph->edges[i][j] = mGraph->edges[i][k] + mGraph->edges[k][j];
                    iArrPath[i][j] = iArrPath[k][j];
                }
            }
        }
    }
}

void obtain_roadcondition(MGraph *mGraph, int **iArrPath){
	//for (int w = 0; w < n_period; w++){
		for (int x = 1; x <= crosses.size(); ++x){                                                  //bias初始化
			for (int y = 1; y <= crosses.size(); ++y){
				bias[x][y] = 0;
			}
		}
		v = cars_num - w*num_each_period;
		while (v <(cars_num - w*num_each_period + 1) && v>(cars_num - (w + 1)*num_each_period + 1) && v >= 0)        //数车
		{

		
		//for (int v = 0; v < cars_num; v++){
			//if (v <(cars_num - w*num_each_period + 1) && v>(cars_num - (w + 1)*num_each_period + 1)){
				// for (int n = 0; n < cars.size(); n++){
				int i = cars_array[v].from, j = cars_array[v].to;
				if (i != j){
					std::stack<int> stackVertices;
					int k = j;
					do
					{
						k = iArrPath[i][k];
						stackVertices.push(k);
					} while (k != i);
					int last = stackVertices.top();
					stackVertices.pop();
					int next;
					//输出路径
					unsigned int nLength = stackVertices.size();
					for (unsigned int nIndex = 0; nIndex < nLength; nIndex++)
					{
						next = stackVertices.top();
						stackVertices.pop();
						bias[last][next] += 1;                                      //获得道路占用次数
						last = next;
					}
					next = j;
					bias[last][next] += 1;
				}
				v--;
				}
	}

void PrintResult(MGraph *mGraph, int **iArrPath,string answer_path){
        //for (int n = 0; n < cars.size(); n++){
	v = cars_num - w*num_each_period;
	while (v <(cars_num - w*num_each_period + 1) && v>(cars_num - (w + 1)*num_each_period + 1)&&v>=0)                //数车
	{
	int i = cars_array[v].from, j = cars_array[v].to;
        if (i != j){
                out <<"("<< cars_array[v].id << "," <<cars_array[v].planTime<< ",";
                    std::stack<int> stackVertices;
                    int k = j;
                    do
                    {
                        k = iArrPath[i][k];
                        stackVertices.push(k);
                    } while (k != i);

                    int last = stackVertices.top();
                    stackVertices.pop();
                    int next;
                    //输出路径
                    unsigned int nLength = stackVertices.size();
                    for (unsigned int nIndex = 0; nIndex < nLength; nIndex++)
                    {
                        next= stackVertices.top();
                        stackVertices.pop();
                        //找到roadID
                        for (int a = 0; a < roads.size();a++)
                        {
                            if ((roads[a].to == last&&roads[a].from == next )||( roads[a].from == last&&roads[a].to == next))
                            {
                                out << roads[a].id << ",";
                            }
                        }
                        last = next;
                    }
                    next = j;
                    for (int a = 0; a < roads.size(); a++)
                    {
                        if ((roads[a].to == last&&roads[a].from == next) || (roads[a].from == last&&roads[a].to == next))
                        {
                            out << roads[a].id << ")"<< endl;
                        }
                    }
                    //out << endl;
                }
			v--;
			}
       }

void Printbais(){
    int max = bias[2][1];
    for (int i = 1; i <= 64; i++){
        for (int j = 1; j <=64; j++){
            //cout << bias[i][j] << " ";
            if (bias[i][j] > max)max = bias[i][j];
        }
        //cout << endl;
    }
    cout << "max_value=" << max << "  "<<endl;                            //最拥挤道路占用次数
}
