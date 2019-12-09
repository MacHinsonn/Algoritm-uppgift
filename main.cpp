#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;
double moldRiskCalc(double avgT, double avgH);
//declaration of the struct Meteo to store data with mixed datatypes together
struct Meteo{
    string date;
    string time;
    bool indoors;
    double temperature;
    int humidity;
};

//function to create struct Meteo
Meteo createMeteo(string date, string time, bool indoors, double temperature, int humidity)
{
    Meteo oneDay;
    oneDay.date = date;
    oneDay.time = time;
    oneDay.indoors = indoors;
    oneDay.temperature = temperature;
    oneDay.humidity = humidity;
    return oneDay;
}

//declares global vector to store the (raw) Meteo structs
vector<Meteo> meteoData;

//struct for averaged values
struct AvgVPD {
    string date;
    double temp;
    double hum;
    double mold;
};
AvgVPD createAvgVPD(string d, double t, double h, double m)
{
    AvgVPD avgVPD;
    avgVPD.date = d;
    avgVPD.hum = h;
    avgVPD.mold = m;
    avgVPD.temp = t;
    return avgVPD;
}

vector<AvgVPD> valuesIndoors;
vector<AvgVPD> valuesOutdoors;

//opens text file and gives error message if it can't be found

ifstream text;
void read_file()
{
    text.open("tempdata4.txt");
    if(text.fail())
    {
        cout << "Could not find data file." << endl;
        exit(1);
    }
        
}



//reads in file data, creates Meteo structs and stores them into a vector
void gothrough()
{
	string day, time, inOut, temperature, humidity;
    bool indoors;
    double temperatureConverted;
    int humidityConverted;

    while(!text.eof())
    {
        getline(text, day, ' ');
        getline(text, time, ',');
        getline(text, inOut, ',');
        getline(text, temperature, ',');
        getline(text, humidity);
        if(inOut.compare("Inne") == 0)
        { 
            indoors = true; 
        } else { indoors = false; } //converts indoors/outdoors status into boolean
        temperatureConverted = stod(temperature); //converts temperature into double
        humidityConverted = stoi(humidity); //converts humidity into int
        Meteo meteo = createMeteo(day, time, indoors, temperatureConverted, humidityConverted);
        meteoData.push_back(meteo);
    }
    
}
// compares Meteo structs on date
bool compareMeteoDate(Meteo left, Meteo right) 
{
    return left.date < right.date;
}

//sort the Meteo vector on date
void sortvector()
{
    sort(meteoData.begin(), meteoData.end(), compareMeteoDate);
}

bool compareMold(AvgVPD left, AvgVPD right)
{
    return left.mold < right.mold;   
}
bool compareHumidity(AvgVPD left, AvgVPD right)
{
    return left.hum < right.hum;   
}
bool compareTemperature(AvgVPD left, AvgVPD right)
{
    return left.temp < right.temp;   
}
//calculates the average values per day for all dates
void avgValuesPerDay()
{
    double sumT;
    double sumH;
    string lastDate = ""; //the last seen date or null
    bool lastIndoors = NULL; //the last seen indoors outdoors or null

    int i = 0;
    int n = 0;

    while(i < meteoData.size())
    {
        Meteo currentMeteo = meteoData[i];
        if(i == 0)
        {
            lastDate = currentMeteo.date;
            lastIndoors = currentMeteo.indoors;
            sumT += currentMeteo.temperature;
            sumH += currentMeteo.humidity;
            n++;
            i++;
        } else if(lastDate == currentMeteo.date && lastIndoors == currentMeteo.indoors)
        {
            sumT += currentMeteo.temperature;
            sumH += currentMeteo.humidity;
            n++;
            i++;
        } else if(lastDate != currentMeteo.date || lastIndoors != currentMeteo.indoors)
        {
            double avgT = sumT / n;
            double avgH = sumH / n;
            double moldRisk = moldRiskCalc(avgT, avgH);
            AvgVPD averages = createAvgVPD(lastDate, avgT, avgH, moldRisk);
            if(lastIndoors)
            {
                valuesIndoors.push_back(averages);
            } else 
            { 
                valuesOutdoors.push_back(averages); 
            }
            lastDate = currentMeteo.date;
            lastIndoors = currentMeteo.indoors;
            sumT = currentMeteo.temperature;
            sumH = currentMeteo.humidity;
            n = 1;
            i++;        
        }
                
    }

}


double moldRiskCalc(double avgT, double avgH)
{  
    double risk;
    if(avgT < 0 || avgH < 78)
    {
        risk = 0;
    } else if(avgT >= 15 && avgH >= 78)
    {
        risk = (avgH - 78) / 0.22;
    } else
    {
        risk = ((avgH - 78) * (avgT / 15)) * 0.22;
    }
    return risk;
}

bool compareDate(AvgVPD left, AvgVPD right)
{
    return left.date < right.date ;
}

AvgVPD whenAutumn()
{
    int i = 0;
    int n = 0;
    AvgVPD autumnStart;
    string lastDate = "";
    double lastTemp = 0;

    while(i < valuesOutdoors.size())
    {
        if(valuesOutdoors[i].temp > valuesOutdoors[i+1].temp)
        {
            lastDate = valuesOutdoors[i].date;
            n += 1;
        }
        i++;
    }
}

AvgVPD whenWinter()
{
    AvgVPD winterStart;
}

void printDay(string date)
{
    AvgVPD indoors;
    AvgVPD outdoors;
    int i = 0;
    int j = 0;

    while(i < valuesIndoors.size())
    {
        if(date == valuesIndoors[i].date)
        {
            indoors = valuesIndoors[i];
        }
        i++;
    }
    while(j < valuesOutdoors.size())
    {
        if(date == valuesOutdoors[j].date)
        {
            outdoors = valuesOutdoors[j];
        }
        j++;
    }
    cout << "On that day: " << endl ;
    cout << "The temperature was " << outdoors.temp << " °C outside and " << indoors.temp << " °C inside.\n" <<
            "The humidity was " << outdoors.hum << " outside and " << indoors.hum << " inside. \n" <<
            "The risk for mold was " << outdoors.mold << " outside and " << indoors.mold << " inside. \n" << endl;
}

int main()
{
    read_file();
    gothrough();
    sortvector(); // always sort vector before other calls calculating values   
    avgValuesPerDay();

    sort(valuesIndoors.begin(), valuesIndoors.end(), compareTemperature);
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareTemperature);
             
    AvgVPD warmestIndoors = valuesIndoors.back(); //warmest has highest temperature value
    AvgVPD coldestIndoors = valuesIndoors.front(); //coldest has lowest temperature value
    AvgVPD warmestOutdoors = valuesOutdoors.back();
    AvgVPD coldestOutdoors = valuesOutdoors.front();


    sort(valuesIndoors.begin(), valuesIndoors.end(), compareMold);
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareMold);

    AvgVPD moldiestIndoors = valuesIndoors.back(); //moldiest has highest mold risk
    AvgVPD moldileastIndoors = valuesIndoors.front(); //moldileast has lowest mold risk
    AvgVPD moldiestOutdoors = valuesOutdoors.back();
    AvgVPD moldileastOutdoors = valuesOutdoors.front();


    sort(valuesIndoors.begin(), valuesIndoors.end(), compareHumidity);
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareHumidity);

    AvgVPD driestIndoors = valuesIndoors.front(); //driest has lowest humidity value
    AvgVPD wettestIndoors = valuesIndoors.back(); //wettest has highest humidity value
    AvgVPD driestOutdoors = valuesOutdoors.front();
    AvgVPD wettestOutdoors = valuesOutdoors.back();
    
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareDate)
    whenAutumn();
    whenWinter();

    string chosenDate;
    //menu
    cout << "OUTDOORS METEOROLOGICAL DATA \n" <<
            "Warmest day: " << warmestOutdoors.date << "; " << warmestOutdoors.temp << " °C || Coldest day: " << coldestOutdoors.date << "; " << coldestOutdoors.temp << " °C. \n" <<
            "Driest day: " << driestOutdoors.date << "; humidity index " << driestOutdoors.hum << " || Most humid day: " << wettestOutdoors.date << "; humidity index " << wettestOutdoors.hum << ". \n" <<
            "Highest risk of mold: " << moldiestOutdoors.date << "; mold index " << moldiestOutdoors.mold << " || Lowest risk of mold: " << moldileastOutdoors.date << "; mold index " << moldileastOutdoors.mold << ". \n" <<
            //"Autumn started on " << autumnStart.date << "and winter started on " << winterStart.date << ". \n" <<
            "INDOORS METEOROLOGICAL DATA \n" <<
            "Warmest day: " << warmestIndoors.date << "; " << warmestIndoors.temp << " °C || Coldest day: " << coldestIndoors.date << "; " << coldestIndoors.temp << " °C. \n" <<
            "Driest day: " << driestIndoors.date << "; humidity index " << driestIndoors.hum << " || Most humid day: " << wettestIndoors.date << "; humidity index " << wettestIndoors.hum << ". \n" <<
            "Highest risk of mold: " << moldiestIndoors.date << "; mold index " << moldiestIndoors.mold << " || Lowest risk of mold: " << moldileastIndoors.date << "; mold index " << moldileastIndoors.mold << ". \n" <<
            "Pick a date to see meteorological data on that day (format must be YYYY-MM-DD) : " << endl;
    cin >> chosenDate;
    printDay(chosenDate);
    return 0;
}
