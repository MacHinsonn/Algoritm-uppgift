#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// declaration of function to calculate the risk for mold
double calculateMoldRisk(double avgT, double avgH);

// declaration of the struct Meteo to store data with mixed datatypes together
struct Meteo
{
    string date;
    string time;
    bool indoors;
    double temperature;
    double humidity;
};

// Meteo constructor
Meteo createMeteo(string date, string time, bool indoors, double temperature, double humidity)
{
    Meteo oneDay;
    oneDay.date = date;
    oneDay.time = time;
    oneDay.indoors = indoors;
    oneDay.temperature = temperature;
    oneDay.humidity = humidity;
    return oneDay;
}

// declaration of global vector to store the (raw) Meteo structs
vector<Meteo> meteoData;

// declaration of the struct to store the averaged values (per day)
struct MeteoVPD
{
    string date;
    double temperature;
    double humidity;
    bool indoors;
    double mold;
};

// MeteoVPD constructor
MeteoVPD createMeteoVPD(string date, double temperature, double humidity, bool indoors, double mold)
{
    MeteoVPD oneDay;
    oneDay.date = date;
    oneDay.temperature = temperature;
    oneDay.humidity = humidity;
    oneDay.indoors = indoors;
    oneDay.mold = mold;
    return oneDay;
}

// declarations of global vectors to store the averaged MeteoVPD structs, split between indoors and outdoors
vector<MeteoVPD> valuesOutdoors;
vector<MeteoVPD> valuesIndoors;

// opens text file and gives error message if it can't be found
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

// reads in file data, creates Meteo structs and stores them into a vector
void gothrough()
{
    string date, time, inOut, temperature, humidity;
    bool indoors;
    double temperatureConverted;
    double humidityConverted;

    while(!text.eof())
    {
        getline(text, date, ' ');
        getline(text, time, ',');
        getline(text, inOut, ',');
        getline(text, temperature, ',');
        getline(text, humidity);

        if(inOut.compare("Inne") == 0)
        {
            indoors = true;
        } else {indoors = false;} // converts indoors/outdoors status into boolean

        temperatureConverted = stod(temperature); // converts temperature into double
        humidityConverted = stod(humidity); // converts humidity into double

        Meteo meteo = createMeteo(date, time, indoors, temperatureConverted, humidityConverted);
        meteoData.push_back(meteo);
    }
}

// compares Meteo structs on date
bool compareMeteoDate(Meteo left, Meteo right)
{
    return left.date < right.date;
}

// compares Meteo structs on indoors/outdoors status
bool compareMeteoIndoors(Meteo left, Meteo right)
{
    if(left.indoors && !right.indoors)
    {
        return true;
    }
    if(!left.indoors && right.indoors)
    {
        return false;
    }
}

// sorts the meteoData vector by date and in/out; stable sort makes sure following sortings keep the order
void sortvector()
{
    stable_sort(meteoData.begin(), meteoData.end(), compareMeteoIndoors);
    stable_sort(meteoData.begin(), meteoData.end(), compareMeteoDate);
}

// calculates the averaged values per day
void calculateAvgVPD()
{
    double sumT;
    double sumH;
    string lastDate = ""; // the last date seen or null
    bool lastIndoors = NULL; // the last seen indoors outdoors or null

    int i = 0;
    int n = 0;

    while(i < meteoData.size())
    {
        Meteo currentMeteo = meteoData[i];
        if(i == 0) // edge case : new day
        {
            lastDate = currentMeteo.date;
            lastIndoors = currentMeteo.indoors;
            sumT += currentMeteo.temperature;
            sumH += currentMeteo.humidity;
            n++;
            i++;
        } else if(lastDate == currentMeteo.date && lastIndoors == currentMeteo.indoors)
        // running case : current day
        {
            sumT += currentMeteo.temperature;
            sumH += currentMeteo.humidity;
            n++;
            i++;

            if(i == meteoData.size()) // edge case: end of vector
            {
                double avgT = sumT / n;
                double avgH = sumH / n;
                double moldRisk = calculateMoldRisk(avgT, avgH);
                MeteoVPD averages = createMeteoVPD(lastDate, avgT, avgH, lastIndoors, moldRisk);

                if(lastIndoors)
                {
                    valuesIndoors.push_back(averages);
                } else
                {
                    valuesOutdoors.push_back(averages);
                }
            }
        } else if(lastDate != currentMeteo.date || lastIndoors != currentMeteo.indoors)
        // edge case : end of day
        {
            double avgT = sumT / n;
            double avgH = sumH / n;

            double moldRisk = calculateMoldRisk(avgT, avgH); // calls function to calculate mold index
            // creates structs of averaged values per day
            MeteoVPD averages = createMeteoVPD(lastDate, avgT, avgH, lastIndoors, moldRisk);
            // stores the averaged structs into an indoors and an outdoors vector
            if(lastIndoors)
            {
                valuesIndoors.push_back(averages);
            } else
            {
                valuesOutdoors.push_back(averages);
            }
            lastDate = currentMeteo.date; // resets lastDate
            lastIndoors = currentMeteo.indoors; // resets lastIndoors
            sumT = currentMeteo.temperature; // resets sumT
            sumH = currentMeteo.humidity; // resets sumH
            n = 1; // resets days counter
            i++;
            /* for(int x = 0; x < valuesOutdoors.size(); x++)
            {
                cout << valuesOutdoors[x].temperature << endl;
            } */
        } 
    }
}

double calculateMoldRisk(double avgT, double avgH)
{
    double risk;

    if(avgT < 0)
    {
        risk = 0;
    } else if(avgH < 78)
    {
        risk = avgH * 0.22;
    } else if(avgT >= 15 && avgH >= 78)
    {
        risk = (avgH - 78) / 0.22;
    } else 
    {
        risk = ((avgH - 78) * (avgT / 15)) * 0.22;
    }
    return risk;
}

// comparisons of MeteoVPD structs on temperature, humidity and mold index
bool compareTemperature(MeteoVPD left, MeteoVPD right)
{
    return left.temperature < right.temperature;
}
bool compareHumidity(MeteoVPD left, MeteoVPD right)
{
    return left.humidity < right.humidity;
}
bool compareMold(MeteoVPD left, MeteoVPD right)
{
    return left.mold < right.mold;
}

// compares MeteoVPD structs on date
bool compareMeteoVPDDate(MeteoVPD left, MeteoVPD right)
{
    return left.date < right.date;
}

// function to determine when autumn starts : five days in a row under 10C and after August 1st
MeteoVPD whenAutumn()
{
    int i = 0;
    int n = 0;
    MeteoVPD autumnStart;
    double maxT = 10;

    while(i < valuesOutdoors.size() && n < 5)
    {
        if(valuesOutdoors[i].temperature >= maxT)
        {
            n = 0; // resets days counter if temperature exceeds 10C
        }
        if(valuesOutdoors[i].temperature < maxT && valuesOutdoors[i].date > "2016-08-01")
        // limitation : only works for a given year (here, 2016)
        {
            if(n == 0)
            {
                autumnStart = valuesOutdoors[i]; // sets autumn start to the first of the five days
            }
            n++;
        }
        i++;
    }
    return autumnStart;
}

// function to determine when winter starts : five days in a row under 5C and after August 1st
MeteoVPD whenWinter()
{
    int i = 0;
    int n = 0;
    MeteoVPD winterStart;
    double maxT = 0;

    while(i < valuesOutdoors.size() && n < 5)
    {
        if(valuesOutdoors[i].temperature >= maxT)
        {
            n = 0;
        }
        if(valuesOutdoors[i].temperature < maxT && valuesOutdoors[i].date > "2016-08-01")
        {
            if(n == 0)
            {
                winterStart = valuesOutdoors[i];
            }
            n++;
        }
        i++;
    }
    return winterStart;
}

// function to display data on a given day picked by user
void printDay(string date)
{
    MeteoVPD indoors;
    MeteoVPD outdoors;
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
    // handles case where date is not in the data :
    // date is initialized to an empty string so if never find one, it stays empty
    if(indoors.date == "" || outdoors.date == "")
    {
        cout << "No data found for this date." << endl;
        return;
    }
    cout << "On that day: " << endl;
    cout << "The temperature was " << outdoors.temperature << " °C outside and " << indoors.temperature <<
            " °C inside.\n " <<
            "The humidity was " << outdoors.humidity << " outside and " << indoors.humidity << " inside.\n" <<
            "The risk for mold was " << outdoors.mold << " outside and " << indoors.mold << " inside." << endl;
}



int main()
{
    read_file();
    gothrough();
    sortvector();
    calculateAvgVPD();

    // running a series of sort and find functions to determine highest and lowest meteorological values
    sort(valuesIndoors.begin(), valuesIndoors.end(), compareTemperature);
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareTemperature);
             
    MeteoVPD warmestIndoors = valuesIndoors.back(); //warmest has highest temperature value
    MeteoVPD coldestIndoors = valuesIndoors.front(); //coldest has lowest temperature value
    MeteoVPD warmestOutdoors = valuesOutdoors.back();
    MeteoVPD coldestOutdoors = valuesOutdoors.front();


    sort(valuesIndoors.begin(), valuesIndoors.end(), compareMold);
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareMold);

    MeteoVPD moldiestIndoors = valuesIndoors.back(); //moldiest has highest mold risk
    MeteoVPD moldileastIndoors = valuesIndoors.front(); //moldileast has lowest mold risk
    MeteoVPD moldiestOutdoors = valuesOutdoors.back();
    MeteoVPD moldileastOutdoors = valuesOutdoors.front();


    sort(valuesIndoors.begin(), valuesIndoors.end(), compareHumidity);
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareHumidity);

    MeteoVPD driestIndoors = valuesIndoors.front(); //driest has lowest humidity value
    MeteoVPD wettestIndoors = valuesIndoors.back(); //wettest has highest humidity value
    MeteoVPD driestOutdoors = valuesOutdoors.front();
    MeteoVPD wettestOutdoors = valuesOutdoors.back();
    
    sort(valuesOutdoors.begin(), valuesOutdoors.end(), compareMeteoVPDDate);
    MeteoVPD autumnStart = whenAutumn();
    MeteoVPD winterStart = whenWinter();

    // menu
    cout << "OUTDOORS METEOROLOGICAL DATA \n" <<
            "Warmest day: " << warmestOutdoors.date << "; " << warmestOutdoors.temperature << 
            " °C || Coldest day: " << coldestOutdoors.date << "; " << coldestOutdoors.temperature << " °C. \n" <<
            "Driest day: " << driestOutdoors.date << "; humidity index " << driestOutdoors.humidity << 
            " || Most humid day: " << wettestOutdoors.date << "; humidity index " << wettestOutdoors.humidity << ". \n" <<
            "Highest risk of mold: " << moldiestOutdoors.date << "; mold index " << moldiestOutdoors.mold << 
            " || Lowest risk of mold: " << moldileastOutdoors.date << "; mold index " << moldileastOutdoors.mold << ". \n" <<
            "Autumn started on " << autumnStart.date << " and winter started on " << winterStart.date << ". \n" <<
            "INDOORS METEOROLOGICAL DATA \n" <<
            "Warmest day: " << warmestIndoors.date << "; " << warmestIndoors.temperature << 
            " °C || Coldest day: " << coldestIndoors.date << "; " << coldestIndoors.temperature << " °C. \n" <<
            "Driest day: " << driestIndoors.date << "; humidity index " << driestIndoors.humidity << 
            " || Most humid day: " << wettestIndoors.date << "; humidity index " << wettestIndoors.humidity << ". \n" <<
            "Highest risk of mold: " << moldiestIndoors.date << "; mold index " << moldiestIndoors.mold << 
            " || Lowest risk of mold: " << moldileastIndoors.date << "; mold index " << moldileastIndoors.mold << ". \n" << endl;

    string chosenDate;

    while(true)
    {
        cout << "Pick a date to see meteorological data on that day (format must be YYYY-MM-DD) : " << endl;
        cin >> chosenDate;
        printDay(chosenDate);
    }

     

    return 0;
}
