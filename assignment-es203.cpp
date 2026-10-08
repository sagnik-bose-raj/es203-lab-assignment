#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <string>
using namespace std;
class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() {}
};
class Crop {
protected:
    string name, season;
    double area, yieldPerAcre;
    static int cropCount;              
public:
    Crop() : name("Unknown"), season("Unknown"), area(1), yieldPerAcre(1) 
	{
        cropCount++;
    }
    Crop(string n, double a, string s, double y) 
	{
        if (a <= 0) throw invalid_argument("Area must be greater than 0.");
        if (y <= 0) throw invalid_argument("Yield per acre must be greater than 0.");
        name = n;
        area = a;
        season = s;
        yieldPerAcre = y;
        cropCount++;
    }
    virtual ~Crop() { cropCount--; }
    static int getCount() { return cropCount; }
    virtual double totalYield() const { return area * yieldPerAcre; }
    virtual string getType() const { return "Crop"; }
    virtual void display() const 
	{
        cout << left << setw(12) << getType()
             << setw(12) << name
             << setw(8)  << fixed << setprecision(2) << area
             << setw(10) << season
             << setw(10) << yieldPerAcre
             << setw(12) << totalYield();
    }
    virtual void save(ofstream &fout) const 
	{
        fout << getType() << " " << name << " " << area << " "
             << season << " " << yieldPerAcre << " ";
    }
    bool operator>(const Crop &c) const  { return totalYield() > c.totalYield(); }
    bool operator<(const Crop &c) const  { return totalYield() < c.totalYield(); }
    bool operator==(const Crop &c) const { return totalYield() == c.totalYield(); }
};
int Crop::cropCount = 0;
class CerealCrop : public Crop, public Advisory 
{
    string fertilizerType;
public:
    CerealCrop() : Crop(), fertilizerType("Urea") {}
    CerealCrop(string n, double a, string s, double y, string f)
        : Crop(n, a, s, y), fertilizerType(f) {}
    string getType() const override { return "Cereal"; }
    void display() const override 
	{
        Crop::display();
        cout << "Fertilizer: " << fertilizerType << endl;
    }
    void save(ofstream &fout) const override 
	{
        Crop::save(fout);
        fout << fertilizerType << endl;
    }
    void giveAdvice() const override 
	{
        cout << "[" << name << "] Apply " << fertilizerType
             << " in split doses and keep the field free of weeds.\n";
    }
};
class VegetableCrop : public Crop, public Advisory 
{
    int irrigationFrequency;         
public:
    VegetableCrop() : Crop(), irrigationFrequency(2) {}
    VegetableCrop(string n, double a, string s, double y, int irr)
        : Crop(n, a, s, y), irrigationFrequency(irr) 
	{
        if (irr <= 0) throw invalid_argument("Irrigation frequency must be greater than 0.");
    }
    string getType() const override { return "Vegetable"; }
    double totalYield() const override { return area * yieldPerAcre * 0.90; }
    void display() const override 
	{
        Crop::display();
        cout << "Irrigation/week: " << irrigationFrequency << endl;
    }
    void save(ofstream &fout) const override 
	{
        Crop::save(fout);
        fout << irrigationFrequency << endl;
    }
    void giveAdvice() const override 
	{
        cout << "[" << name << "] Water " << irrigationFrequency
             << " times a week and check regularly for pests.\n";
    }
};
void clearCrops(vector<Crop*> &crops) 
{
    for (Crop *c : crops) delete c;
    crops.clear();
}
void saveToFile(const vector<Crop*> &crops) 
{
    ofstream fout("crops.txt");
    if (!fout) throw runtime_error("Could not open crops.txt for writing.");
    for (const Crop *c : crops) c->save(fout);
    cout << "Saved " << crops.size() << " crops to crops.txt\n";
}
void loadFromFile(vector<Crop*> &crops) 
{
    ifstream fin("crops.txt");
    if (!fin) throw runtime_error("crops.txt not found. Save some data first.");
    clearCrops(crops);
    string type, name, season, extra;
    double area, y;
    while (fin >> type >> name >> area >> season >> y >> extra) 
	{
        try 
		{
            if (type == "Cereal")
                crops.push_back(new CerealCrop(name, area, season, y, extra));
            else if (type == "Vegetable")
                crops.push_back(new VegetableCrop(name, area, season, y, stoi(extra)));
        } catch (exception &e) {
            cout << "Skipped a bad record: " << e.what() << endl;
        }
    }
    cout << "Loaded " << crops.size() << " crops from file.\n";
}
void showAll(const vector<Crop*> &crops) 
{
    if (crops.empty()) 
	{
        cout << "No crops added yet.\n";
        return;
    }
    cout << left << setw(12) << "Type" << setw(12) << "Name" << setw(8) << "Area"
         << setw(10) << "Season" << setw(10) << "Yield/Ac" << setw(12) << "TotalYield" << endl;
    cout << string(75, '-') << endl;
    double grand = 0;
    for (Crop *c : crops) 
	{
        c->display();                
        grand += c->totalYield();
    }
    cout << "\nTotal crops: " << Crop::getCount()
         << " | Grand total yield: " << grand << endl;
}
int main() 
{
    vector<Crop*> crops;
    int choice = 0;
    do 
	{
        cout << "\n===== SMART FARM MANAGEMENT SYSTEM =====\n"
             << "1. Add Cereal Crop\n"
             << "2. Add Vegetable Crop\n"
             << "3. Display All Crops\n"
             << "4. Compare Two Crops\n"
             << "5. Get Farming Advice\n"
             << "6. Save to File\n"
             << "7. Load from File\n"
             << "8. Exit\n"
             << "Enter choice: ";
        cin >> choice;
        try 
		{
            if (cin.fail()) throw invalid_argument("Please enter a number.");
            if (choice == 1 || choice == 2) 
			{
                string n, s, extra;
                double a, y;
                int irr = 0;
                cout << "Name (one word): ";   cin >> n;
                cout << "Area (acres): ";      cin >> a;
                cout << "Season: ";            cin >> s;
                cout << "Yield per acre: ";    cin >> y;
                if (choice == 1) 
				{
                    cout << "Fertilizer type: "; cin >> extra;
                    if (cin.fail()) throw invalid_argument("Invalid input.");
                    crops.push_back(new CerealCrop(n, a, s, y, extra));
                } else {
                    cout << "Irrigation frequency (per week): "; cin >> irr;
                    if (cin.fail()) throw invalid_argument("Invalid input.");
                    crops.push_back(new VegetableCrop(n, a, s, y, irr));
                }
                cout << "Crop added.\n";
            }
            else if (choice == 3) 
			{
                showAll(crops);
            }
            else if (choice == 4) 
			{
                if (crops.size() < 2) throw runtime_error("You need at least 2 crops to compare.");
                int i, j;
                cout << "Enter two crop numbers (1-" << crops.size() << "): ";
                cin >> i >> j;
                if (cin.fail() || i < 1 || j < 1 || i > (int)crops.size() || j > (int)crops.size())
                    throw out_of_range("Invalid crop number.");
                Crop &first = *crops[i - 1];
                Crop &second = *crops[j - 1];
                if (first > second)      cout << "Crop " << i << " has the higher yield.\n";
                else if (first < second) cout << "Crop " << j << " has the higher yield.\n";
                else                     cout << "Both crops have equal yield.\n";
            }
            else if (choice == 5) 
			{
                if (crops.empty()) cout << "No crops added yet.\n";
                for (Crop *c : crops)
				{
                    Advisory *adv = dynamic_cast<Advisory*>(c);
                    if (adv) adv->giveAdvice();
                }
            }
            else if (choice == 6) saveToFile(crops);
            else if (choice == 7) loadFromFile(crops);
            else if (choice != 8) cout << "Invalid choice.\n";
        }
        catch (exception &e)
	    {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            if (cin.eof()) break;
        }
    } 
	while (choice != 8);
    clearCrops(crops);
    cout << "Goodbye.\n";
    return 0;
}
