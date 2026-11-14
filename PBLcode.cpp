#include<iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <limits>
using namespace std;


int readInt(string message, int minimum, int maximum) {
    int value;

    while (true) {
        cout << message;

        if (cin >> value && value >= minimum && value <= maximum) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Invalid input! Please try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}


class Vehicle {
private:
    int vehicleId;
    string type;
    int arrivalOrder;
    int waitingTime;

public:
    Vehicle(int id, string t, int order) {
        vehicleId = id;
        type = t;
        arrivalOrder = order;
        waitingTime = 0;
    }

    virtual bool isEmergency() const {
        return false;
    }

    virtual int getPriority() const {
        return 0;
    }

    int getId() const {
        return vehicleId;
    }

    string getType() const {
        return type;
    }

    int getArrivalOrder() const {
        return arrivalOrder;
    }

    int getWaitingTime() const {
        return waitingTime;
    }

    void increaseWaitingTime() {
        waitingTime++;
    }

    virtual void displayDetails() const {
        cout << "ID: " << vehicleId
             << " | Type: " << type
             << " | Waiting cycles: " << waitingTime;
    }

    virtual ~Vehicle() {}
};


class EmergencyVehicle : public Vehicle {
private:
    int priorityLevel;

public:
    EmergencyVehicle(int id, string t, int priority, int order)
        : Vehicle(id, t, order) {
        priorityLevel = priority;
    }

    bool isEmergency() const override {
        return true;
    }

    int getPriority() const override {
        return priorityLevel;
    }

    void displayDetails() const override {
        cout << "Emergency ID: " << getId()
             << " | Type: " << getType()
             << " | Priority: " << priorityLevel
             << " | Waiting cycles: " << getWaitingTime();
    }
};


struct Node {
    Vehicle* vehicle;
    Node* next;

    Node(Vehicle* v) {
        vehicle = v;
        next = nullptr;
    }
};


class LaneQueue {
private:
    Node* frontPtr;
    Node* rearPtr;
    int count;

public:
    LaneQueue() {
        frontPtr = nullptr;
        rearPtr = nullptr;
        count = 0;
    }

    bool isEmpty() const {
        return frontPtr == nullptr;
    }

    int getSize() const {
        return count;
    }


    void enqueue(Vehicle* v) {
        Node* newNode = new Node(v);

        if (isEmpty()) {
            frontPtr = rearPtr = newNode;
        }
        else {
            rearPtr->next = newNode;
            rearPtr = newNode;
        }

        count++;
    }


    Vehicle* popFront() {
        if (isEmpty()) {
            return nullptr;
        }

        Node* temp = frontPtr;
        Vehicle* v = temp->vehicle;

        frontPtr = frontPtr->next;

        if (frontPtr == nullptr) {
            rearPtr = nullptr;
        }

        delete temp;
        count--;

        return v;
    }


    bool containsId(int id) const {
        Node* current = frontPtr;

        while (current != nullptr) {
            if (current->vehicle->getId() == id) {
                return true;
            }

            current = current->next;
        }

        return false;
    }


    Vehicle* findBestEmergency() const {
        Node* current = frontPtr;
        Vehicle* best = nullptr;

        while (current != nullptr) {
            Vehicle* v = current->vehicle;

            if (v->isEmergency()) {
                if (best == nullptr ||
                    v->getPriority() > best->getPriority() ||
                    (v->getPriority() == best->getPriority() &&
                     v->getArrivalOrder() < best->getArrivalOrder())) {

                    best = v;
                }
            }

            current = current->next;
        }

        return best;
    }


    Vehicle* removeByArrivalOrder(int order) {
        Node* current = frontPtr;
        Node* previous = nullptr;

        while (current != nullptr) {
            if (current->vehicle->getArrivalOrder() == order) {

                if (previous == nullptr) {
                    frontPtr = current->next;
                }
                else {
                    previous->next = current->next;
                }

                if (current == rearPtr) {
                    rearPtr = previous;
                }

                Vehicle* v = current->vehicle;

                delete current;
                count--;

                return v;
            }

            previous = current;
            current = current->next;
        }

        return nullptr;
    }

    bool hasEmergency() const {
        return findBestEmergency() != nullptr;
    }


    void updateWaitingTimes() {
        Node* current = frontPtr;

        while (current != nullptr) {
            current->vehicle->increaseWaitingTime();
            current = current->next;
        }
    }

    int getMaximumWaitingTime() const {
        int maximum = 0;
        Node* current = frontPtr;

        while (current != nullptr) {
            if (current->vehicle->getWaitingTime() > maximum) {
                maximum = current->vehicle->getWaitingTime();
            }

            current = current->next;
        }

        return maximum;
    }

    double getAverageWaitingTime() const {
        if (isEmpty()) {
            return 0;
        }

        int total = 0;
        Node* current = frontPtr;

        while (current != nullptr) {
            total += current->vehicle->getWaitingTime();
            current = current->next;
        }

        return (double)total / count;
    }

    void displayQueue() const {
        Node* current = frontPtr;

        if (isEmpty()) {
            cout << "No vehicles waiting.\n";
            return;
        }

        while (current != nullptr) {
            current->vehicle->displayDetails();
            cout << endl;
            current = current->next;
        }
    }

    void displayEmergencies() const {
        Node* current = frontPtr;

        while (current != nullptr) {
            if (current->vehicle->isEmergency()) {
                current->vehicle->displayDetails();
                cout << endl;
            }

            current = current->next;
        }
    }

    ~LaneQueue() {
        while (!isEmpty()) {
            delete popFront();
        }
    }
};

class TrafficSignal {
private:
    string laneName;
    string signal;
    int greenTime;

public:
    TrafficSignal() {
        laneName = "";
        signal = "RED";
        greenTime = 10;
    }

    void setLane(string name) {
        laneName = name;
    }

    void setSignal(string state) {
        signal = state;
    }

    void calculateGreenTime(int vehicles) {
        greenTime = 10 + vehicles * 2;

        if (greenTime > 60) {
            greenTime = 60;
        }
    }

    void display(int vehicles) const {
        cout << left << setw(12) << laneName
             << setw(12) << vehicles
             << setw(12) << signal
             << greenTime << " sec\n";
    }
};


struct TrafficRecord {
    int vehicleId;
    string laneName;
    string vehicleType;
    bool emergency;
};


class RecordStack {
private:
    static const int MAX = 200;
    TrafficRecord records[MAX];
    int top;

public:
    RecordStack() {
        top = -1;
    }

    bool isEmpty() const {
        return top == -1;
    }

    void push(TrafficRecord r) {
        if (top == MAX - 1) {
            cout << "History stack is full!\n";
            return;
        }

        records[++top] = r;
    }

    void displayRecords() const {
        cout << "\n========== RECENT DEPARTURE HISTORY ==========\n";

        if (isEmpty()) {
            cout << "No departure records available.\n";
            return;
        }

        for (int i = top; i >= 0; i--) {
            cout << "ID: " << records[i].vehicleId
                 << " | Lane: " << records[i].laneName
                 << " | Type: " << records[i].vehicleType
                 << " | Category: "
                 << (records[i].emergency ? "Emergency" : "Normal")
                 << endl;
        }
    }
};


void saveRecordToFile(TrafficRecord r) {
    ofstream file("traffic_records.txt", ios::app);

    if (!file) {
        cout << "Unable to open record file.\n";
        return;
    }

    file << "ID: " << r.vehicleId
         << " | Lane: " << r.laneName
         << " | Type: " << r.vehicleType
         << " | Category: "
         << (r.emergency ? "Emergency" : "Normal")
         << endl;

    file.close();
}


class TrafficManagementSystem {
private:
    static const int N = 4;

    string laneNames[N];

    LaneQueue lanes[N];
    TrafficSignal signals[N];

    RecordStack history;

    int nextArrivalOrder;

    int totalDepartures;
    int emergencyDepartures;
    int normalDepartures;
    int laneDepartures[N];

    bool pedestrianRequested;


    int findBusiestLane() const {
        int busiest = -1;

        for (int i = 0; i < N; i++) {
            if (lanes[i].isEmpty()) {
                continue;
            }

            if (busiest == -1 ||
                lanes[i].getSize() > lanes[busiest].getSize()) {

                busiest = i;
            }
        }

        return busiest;
    }

    int chooseNextLane() const {
        int selected = -1;
        int highestWait = -1;

        for (int i = 0; i < N; i++) {
            if (lanes[i].isEmpty()) {
                continue;
            }

            int waiting = lanes[i].getMaximumWaitingTime();

            if (waiting > highestWait ||
                (waiting == highestWait &&
                 selected != -1 &&
                 lanes[i].getSize() > lanes[selected].getSize())) {

                highestWait = waiting;
                selected = i;
            }
        }



        if (highestWait >= 3) {
            return selected;
        }

        return findBusiestLane();
    }



    bool emergencyWaiting() const {
        for (int i = 0; i < N; i++) {
            if (lanes[i].hasEmergency()) {
                return true;
            }
        }

        return false;
    }



    void setAllRed() {
        for (int i = 0; i < N; i++) {
            signals[i].setSignal("RED");
        }
    }


    void updateNormalSignals() {
        for (int i = 0; i < N; i++) {
            signals[i].calculateGreenTime(lanes[i].getSize());
        }

        setAllRed();


        if (emergencyWaiting() || pedestrianRequested) {
            return;
        }

        int selected = chooseNextLane();

        if (selected != -1) {
            signals[selected].setSignal("GREEN");
        }
    }

    bool idExists(int id) const {
        for (int i = 0; i < N; i++) {
            if (lanes[i].containsId(id)) {
                return true;
            }
        }

        return false;
    }


    int getUniqueId() const {
        while (true) {
            int id = readInt("Enter vehicle ID: ", 1, 999999);

            if (!idExists(id)) {
                return id;
            }

            cout << "This ID already exists. Enter another ID.\n";
        }
    }


    void recordDeparture(Vehicle* v, int lane) {
        TrafficRecord r;

        r.vehicleId = v->getId();
        r.laneName = laneNames[lane];
        r.vehicleType = v->getType();
        r.emergency = v->isEmergency();

        history.push(r);
        saveRecordToFile(r);

        totalDepartures++;
        laneDepartures[lane]++;

        if (r.emergency) {
            emergencyDepartures++;
        }
        else {
            normalDepartures++;
        }

        delete v;
    }


    void displayAllLanes() const {
        cout << "\n========== VEHICLE LANE DETAILS ==========\n";

        for (int i = 0; i < N; i++) {
            cout << "\n" << laneNames[i] << " Lane\n";
            cout << "Vehicles waiting: " << lanes[i].getSize() << endl;

            lanes[i].displayQueue();
        }
    }

public:


    TrafficManagementSystem() {
        laneNames[0] = "North";
        laneNames[1] = "South";
        laneNames[2] = "East";
        laneNames[3] = "West";

        nextArrivalOrder = 1;

        totalDepartures = 0;
        emergencyDepartures = 0;
        normalDepartures = 0;

        pedestrianRequested = false;

        for (int i = 0; i < N; i++) {
            signals[i].setLane(laneNames[i]);
            laneDepartures[i] = 0;
        }
    }


    void addNormalVehicle() {
        cout << "\n========== ADD NORMAL VEHICLE ==========\n";

        int lane = readInt(
            "Select lane (1-North, 2-South, 3-East, 4-West): ",
            1, 4
        );

        lane--;

        int id = getUniqueId();

        cout << "\nVehicle Types\n";
        cout << "1. Car\n";
        cout << "2. Bike\n";
        cout << "3. Bus\n";
        cout << "4. Truck\n";

        int typeChoice = readInt("Select vehicle type: ", 1, 4);

        string type;

        switch (typeChoice) {
            case 1: type = "Car"; break;
            case 2: type = "Bike"; break;
            case 3: type = "Bus"; break;
            case 4: type = "Truck"; break;
        }

        Vehicle* v = new Vehicle(id, type, nextArrivalOrder++);

        lanes[lane].enqueue(v);

        updateNormalSignals();

        cout << "\nVehicle added successfully to "
             << laneNames[lane] << " lane.\n";
    }

    void addEmergencyVehicle() {
        cout << "\n========== ADD EMERGENCY VEHICLE ==========\n";

        int lane = readInt(
            "Select lane (1-North, 2-South, 3-East, 4-West): ",
            1, 4
        );

        lane--;

        int id = getUniqueId();

        cout << "\nEmergency Vehicle Types\n";
        cout << "1. Ambulance (Priority 3)\n";
        cout << "2. Fire Truck (Priority 2)\n";
        cout << "3. Police Vehicle (Priority 1)\n";

        int choice = readInt("Select emergency type: ", 1, 3);

        string type;
        int priority;

        switch (choice) {
            case 1:
                type = "Ambulance";
                priority = 3;
                break;

            case 2:
                type = "Fire Truck";
                priority = 2;
                break;

            case 3:
                type = "Police";
                priority = 1;
                break;
        }

        Vehicle* v = new EmergencyVehicle(
            id, type, priority, nextArrivalOrder++
        );

        lanes[lane].enqueue(v);

        updateNormalSignals();

        cout << "\nEmergency vehicle added successfully!\n";
        cout << "Emergency priority: " << priority << endl;
    }

    void vehicleMenu() {
        int choice;

        do {
            cout << "\n========== VEHICLE MANAGEMENT ==========\n";
            cout << "1. Add Normal Vehicle\n";
            cout << "2. Add Emergency Vehicle\n";
            cout << "3. Display All Lanes\n";
            cout << "0. Back to Main Menu\n";

            choice = readInt("Enter choice: ", 0, 3);

            switch (choice) {
                case 1:
                    addNormalVehicle();
                    break;

                case 2:
                    addEmergencyVehicle();
                    break;

                case 3:
                    displayAllLanes();
                    break;
            }

        } while (choice != 0);
    }


    void showSignalReport() {
        updateNormalSignals();

        cout << "\n";
        cout << "================================================\n";
        cout << "              TRAFFIC SIGNAL REPORT             \n";
        cout << "================================================\n";

        cout << left << setw(12) << "LANE"
             << setw(12) << "VEHICLES"
             << setw(12) << "SIGNAL"
             << "GREEN TIME\n";

        cout << "------------------------------------------------\n";

        for (int i = 0; i < N; i++) {
            signals[i].display(lanes[i].getSize());
        }

        cout << "================================================\n";

        if (emergencyWaiting()) {
            cout << "Emergency request pending. Handle emergency first.\n";
        }
        else if (pedestrianRequested) {
            cout << "Pedestrian crossing is pending.\n";
        }
    }

    void processNormalDeparture() {
        if (emergencyWaiting()) {
            cout << "\nEmergency vehicle is waiting!\n";
            cout << "Normal traffic is temporarily stopped.\n";
            return;
        }

        if (pedestrianRequested) {
            cout << "\nPedestrian crossing is pending!\n";
            cout << "Complete pedestrian crossing first.\n";
            return;
        }

        int lane = chooseNextLane();

        if (lane == -1) {
            cout << "\nNo vehicles are waiting in any lane.\n";
            updateNormalSignals();
            return;
        }

        signals[lane].setSignal("GREEN");

        cout << "\nGREEN signal for " << laneNames[lane] << " lane.\n";
        cout << "Green duration: "
             << 10 + lanes[lane].getSize() * 2
             << " seconds (calculated adaptive duration).\n";

        cout << "\nSignal changing to YELLOW...\n";
        signals[lane].setSignal("YELLOW");

        Vehicle* v = lanes[lane].popFront();

        cout << "\nVehicle departed successfully:\n";
        v->displayDetails();
        cout << endl;

        recordDeparture(v, lane);

        for (int i = 0; i < N; i++) {
            lanes[i].updateWaitingTimes();
        }

        updateNormalSignals();

        cout << "\nSignal operation completed.\n";
    }

    void signalMenu() {
        int choice;

        do {
            cout << "\n========== TRAFFIC SIGNAL MANAGEMENT ==========\n";
            cout << "1. View Signal Report\n";
            cout << "2. Process Normal Vehicle Departure\n";
            cout << "0. Back to Main Menu\n";

            choice = readInt("Enter choice: ", 0, 2);

            switch (choice) {
                case 1:
                    showSignalReport();
                    break;

                case 2:
                    processNormalDeparture();
                    break;
            }

        } while (choice != 0);
    }

    void showEmergencyRequests() const {
        cout << "\n========== EMERGENCY REQUESTS ==========\n";

        bool found = false;

        for (int i = 0; i < N; i++) {
            cout << "\n" << laneNames[i] << " Lane:\n";

            if (lanes[i].hasEmergency()) {
                lanes[i].displayEmergencies();
                found = true;
            }
            else {
                cout << "No emergency vehicles.\n";
            }
        }

        if (!found) {
            cout << "\nNo emergency requests are pending.\n";
        }
    }

    void handleEmergency() {
        int bestLane = -1;
        Vehicle* bestVehicle = nullptr;


        for (int i = 0; i < N; i++) {
            Vehicle* candidate = lanes[i].findBestEmergency();

            if (candidate == nullptr) {
                continue;
            }

            if (bestVehicle == nullptr ||
                candidate->getPriority() > bestVehicle->getPriority() ||
                (candidate->getPriority() == bestVehicle->getPriority() &&
                 candidate->getArrivalOrder() <
                 bestVehicle->getArrivalOrder())) {

                bestVehicle = candidate;
                bestLane = i;
            }
        }

        if (bestVehicle == nullptr) {
            cout << "\nNo emergency vehicles are waiting.\n";
            return;
        }


        int selectedOrder = bestVehicle->getArrivalOrder();

        Vehicle* selected = lanes[bestLane].removeByArrivalOrder(
            selectedOrder
        );

        setAllRed();

        cout << "\n========== EMERGENCY OVERRIDE ==========\n";

        signals[bestLane].setSignal("GREEN");

        cout << "GREEN signal granted to "
             << laneNames[bestLane] << " lane.\n";

        cout << "\nSelected emergency vehicle:\n";
        selected->displayDetails();
        cout << endl;

        cout << "\nEmergency vehicle is passing...\n";

        signals[bestLane].setSignal("YELLOW");
        cout << "Signal changing to YELLOW...\n";

        setAllRed();

        cout << "All signals returned to RED temporarily.\n";

        recordDeparture(selected, bestLane);

        for (int i = 0; i < N; i++) {
            lanes[i].updateWaitingTimes();
        }

        cout << "\nEmergency vehicle served and recorded.\n";


        updateNormalSignals();
    }

    void emergencyMenu() {
        int choice;

        do {
            cout << "\n========== EMERGENCY MANAGEMENT ==========\n";
            cout << "1. View Emergency Requests\n";
            cout << "2. Serve Highest Priority Emergency\n";
            cout << "0. Back to Main Menu\n";

            choice = readInt("Enter choice: ", 0, 2);

            switch (choice) {
                case 1:
                    showEmergencyRequests();
                    break;

                case 2:
                    handleEmergency();
                    break;
            }

        } while (choice != 0);
    }


    string getDensity(int vehicles) const {
        if (vehicles <= 2) {
            return "LOW";
        }
        else if (vehicles <= 5) {
            return "MEDIUM";
        }

        return "HIGH";
    }

    void showTrafficMonitoring() const {
        cout << "\n";
        cout << "====================================================\n";
        cout << "              TRAFFIC MONITORING REPORT             \n";
        cout << "====================================================\n";

        cout << left << setw(12) << "LANE"
             << setw(12) << "VEHICLES"
             << setw(12) << "DENSITY"
             << setw(12) << "AVG WAIT"
             << "STATUS\n";

        cout << "----------------------------------------------------\n";

        for (int i = 0; i < N; i++) {
            int count = lanes[i].getSize();

            string density = getDensity(count);

            double averageWait = lanes[i].getAverageWaitingTime();

            bool congested =
                count >= 6 ||
                lanes[i].getMaximumWaitingTime() >= 3;

            cout << left << setw(12) << laneNames[i]
                 << setw(12) << count
                 << setw(12) << density
                 << setw(12) << fixed << setprecision(1)
                 << averageWait
                 << (congested ? "CONGESTED" : "NORMAL")
                 << endl;
        }

        cout << "====================================================\n";
    }

    void showDashboard() const {
        int totalWaiting = 0;

        for (int i = 0; i < N; i++) {
            totalWaiting += lanes[i].getSize();
        }

        cout << "\n========== TRAFFIC DASHBOARD ==========\n";

        cout << "Total vehicles waiting: " << totalWaiting << endl;
        cout << "Total departures: " << totalDepartures << endl;
        cout << "Normal departures: " << normalDepartures << endl;
        cout << "Emergency departures: " << emergencyDepartures << endl;

        cout << "Pending emergency: "
             << (emergencyWaiting() ? "YES" : "NO") << endl;

        cout << "Pedestrian request: "
             << (pedestrianRequested ? "YES" : "NO") << endl;

        showTrafficMonitoring();
    }

    void monitoringMenu() {
        int choice;

        do {
            cout << "\n========== TRAFFIC MONITORING ==========\n";
            cout << "1. Traffic Dashboard\n";
            cout << "2. Density and Congestion Report\n";
            cout << "0. Back to Main Menu\n";

            choice = readInt("Enter choice: ", 0, 2);

            switch (choice) {
                case 1:
                    showDashboard();
                    break;

                case 2:
                    showTrafficMonitoring();
                    break;
            }

        } while (choice != 0);
    }


    void requestPedestrianCrossing() {
        if (pedestrianRequested) {
            cout << "\nA pedestrian request is already pending.\n";
            return;
        }

        pedestrianRequested = true;

        updateNormalSignals();

        cout << "\nPedestrian crossing requested successfully.\n";

        if (emergencyWaiting()) {
            cout << "Crossing will wait until emergency traffic is served.\n";
        }
        else {
            cout << "All vehicle signals will remain RED during crossing.\n";
        }
    }

    void processPedestrianCrossing() {
        if (!pedestrianRequested) {
            cout << "\nNo pedestrian request is pending.\n";
            return;
        }

        if (emergencyWaiting()) {
            cout << "\nEmergency traffic must be served first.\n";
            return;
        }

        setAllRed();

        cout << "\n========== PEDESTRIAN CROSSING ==========\n";

        cout << "All vehicle signals: RED\n";
        cout << "Pedestrian signal: WALK\n";
        cout << "Crossing duration: 5 seconds (simulated).\n";

        cout << "\nPedestrian crossing completed.\n";

        pedestrianRequested = false;

        cout << "Pedestrian signal: DON'T WALK\n";

        updateNormalSignals();

        cout << "Normal traffic operation restored.\n";
    }

    void pedestrianMenu() {
        int choice;

        do {
            cout << "\n========== PEDESTRIAN MANAGEMENT ==========\n";
            cout << "1. Request Pedestrian Crossing\n";
            cout << "2. Process Pedestrian Crossing\n";
            cout << "0. Back to Main Menu\n";

            choice = readInt("Enter choice: ", 0, 2);

            switch (choice) {
                case 1:
                    requestPedestrianCrossing();
                    break;

                case 2:
                    processPedestrianCrossing();
                    break;
            }

        } while (choice != 0);
    }


    void showStatistics() const {
        cout << "\n========== DEPARTURE STATISTICS ==========\n";

        cout << "Total departures: " << totalDepartures << endl;
        cout << "Normal vehicles: " << normalDepartures << endl;
        cout << "Emergency vehicles: " << emergencyDepartures << endl;

        cout << "\nLane-wise departures:\n";

        for (int i = 0; i < N; i++) {
            cout << laneNames[i] << ": "
                 << laneDepartures[i] << " vehicles\n";
        }
    }

    void showSavedFileRecords() const {
        ifstream file("traffic_records.txt");

        cout << "\n========== SAVED DEPARTURE RECORDS ==========\n";

        if (!file) {
            cout << "No saved record file found yet.\n";
            return;
        }

        string line;
        bool found = false;

        while (getline(file, line)) {
            cout << line << endl;
            found = true;
        }

        if (!found) {
            cout << "No records available in file.\n";
        }

        file.close();
    }

    void reportsMenu() {
        int choice;

        do {
            cout << "\n========== REPORTS AND HISTORY ==========\n";
            cout << "1. Recent Departure History (Stack)\n";
            cout << "2. Departure Statistics\n";
            cout << "3. View Saved File Records\n";
            cout << "0. Back to Main Menu\n";

            choice = readInt("Enter choice: ", 0, 3);

            switch (choice) {
                case 1:
                    history.displayRecords();
                    break;

                case 2:
                    showStatistics();
                    break;

                case 3:
                    showSavedFileRecords();
                    break;
            }

        } while (choice != 0);
    }


    void loadSampleTraffic() {
        int total = 0;

        for (int i = 0; i < N; i++) {
            total += lanes[i].getSize();
        }

        if (total > 0) {
            cout << "\nSample data can only be loaded when all lanes are empty.\n";
            return;
        }

        lanes[0].enqueue(new Vehicle(101, "Car", nextArrivalOrder++));
        lanes[0].enqueue(new Vehicle(102, "Bike", nextArrivalOrder++));

        lanes[1].enqueue(new Vehicle(201, "Bus", nextArrivalOrder++));
        lanes[1].enqueue(new EmergencyVehicle(
            202, "Ambulance", 3, nextArrivalOrder++
        ));

        lanes[2].enqueue(new Vehicle(301, "Car", nextArrivalOrder++));
        lanes[2].enqueue(new Vehicle(302, "Bus", nextArrivalOrder++));
        lanes[2].enqueue(new Vehicle(303, "Bike", nextArrivalOrder++));

        lanes[3].enqueue(new EmergencyVehicle(
            401, "Fire Truck", 2, nextArrivalOrder++
        ));

        updateNormalSignals();

        cout << "\nSample traffic loaded successfully!\n";
        cout << "Four lanes now contain normal and emergency traffic.\n";
    }


    void displayMainMenu() const {
        cout << "\n\n";
        cout << "====================================================\n";
        cout << "             SMART TRAFFIC SYSTEM                   \n";
        cout << "       INTELLIGENT INTERSECTION MANAGEMENT          \n";
        cout << "====================================================\n";

        cout << "                    MAIN MENU                       \n";
        cout << "----------------------------------------------------\n";

        cout << "1. Vehicle Management\n";
        cout << "2. Traffic Signal Management\n";
        cout << "3. Emergency Vehicle Management\n";
        cout << "4. Traffic Monitoring\n";
        cout << "5. Pedestrian Crossing\n";
        cout << "6. Reports and History\n";
        cout << "7. Load Sample Traffic\n";
        cout << "0. Exit System\n";

        cout << "----------------------------------------------------\n";
    }

    void run() {
        int choice;

        do {
            displayMainMenu();

            choice = readInt("Enter your choice: ", 0, 7);

            switch (choice) {
                case 1:
                    vehicleMenu();
                    break;

                case 2:
                    signalMenu();
                    break;

                case 3:
                    emergencyMenu();
                    break;

                case 4:
                    monitoringMenu();
                    break;

                case 5:
                    pedestrianMenu();
                    break;

                case 6:
                    reportsMenu();
                    break;

                case 7:
                    loadSampleTraffic();
                    break;

                case 0:
                    cout << "\nExiting Smart Traffic System.\n";
                    cout << "Thank you for using our project!\n";
                    break;
            }

        } while (choice != 0);
    }
};


int main() {
    TrafficManagementSystem system;

    cout << "\nWelcome to Smart Traffic Intersection System!\n";

    system.run();

    return 0;
}
