#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =====================================================
   STRUCTURE DEFINITIONS
   ===================================================== */

typedef struct {
    int id;
    char name[50];
    char phone[15];
} Customer;

// Existing Ticket struct (for History)
typedef struct {
    int ticketId;
    int customerId;
    char issue[100];
    char type[20]; 
} Ticket;

const char *serviceList[7] = { 
    "1. Billing Inquiries",
    "2. Technical Repair",
    "3. New Connection",
    "4. Priority Support (Issue Handling)",
    "5. Device Exchange Hub (Trade-Ins)", // CHANGED
    "6. Smart Serve (De-escalation)",
    "7. Fast-Track VIP Trade-Ins"        // CHANGED
};

/* =====================================================
   DATA STRUCTURE NODES
   ====================================================== */

typedef struct BSTNode {
    Customer customer;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct Node {
    Ticket ticket;
    struct Node *next;
} Node;

typedef struct FeedbackNode {
    int customerId;
    char message[200];
    struct FeedbackNode *next;
} FeedbackNode;

// Node for Index 4 (Priority Support)
typedef struct PTNode {
    char custId[20]; 
    char name[50];
    char issueType[20]; 
    char mood[20];      
    int priority;       
    int score;
    struct PTNode *next;
} PTNode;

// NEW: Node for Index 5 (Device Trade-In Deque)
typedef struct TradeNode {
    int customerId;
    char deviceName[50];
    char condition[20]; // "Mint", "Good", "Damaged"
    int year;
    struct TradeNode *next;
    struct TradeNode *prev; // Doubly Linked
} TradeNode;

/* ======================================================
   GLOBAL POINTERS
   ======================================================= */

BSTNode *customerRoot = NULL;       
Node *completedStackTop = NULL;     
FeedbackNode *feedbackHead = NULL;  

// Pointers for Index 4
PTNode *highHead = NULL, *highTail = NULL;
PTNode *medHead = NULL, *medTail = NULL;
PTNode *lowHead = NULL, *lowTail = NULL;

// NEW: Pointers for Index 5 (Deque)
TradeNode *tradeFront = NULL;
TradeNode *tradeRear = NULL;

int globalTicketCounter = 100;

/* ====================================================
   HELPER FUNCTIONS (BST, STACK)
   =====================================================*/

BSTNode* createBSTNode(int id, char *name, char *phone) {
    BSTNode *newNode = (BSTNode*)malloc(sizeof(BSTNode));
    newNode->customer.id = id;
    strcpy(newNode->customer.name, name);
    strcpy(newNode->customer.phone, phone);
    newNode->left = newNode->right = NULL;
    return newNode;
}

BSTNode* insertCustomer(BSTNode *root, int id, char *name, char *phone) {
    if (root == NULL) return createBSTNode(id, name, phone);
    if (id < root->customer.id)
        root->left = insertCustomer(root->left, id, name, phone);
    else if (id > root->customer.id)
        root->right = insertCustomer(root->right, id, name, phone);
    return root;
}

BSTNode* searchCustomer(BSTNode *root, int id) {
    if (root == NULL || root->customer.id == id) return root;
    if (id < root->customer.id) return searchCustomer(root->left, id);
    return searchCustomer(root->right, id);
}

void displayAllCustomers(BSTNode *root) {
    if (root != NULL) {
        displayAllCustomers(root->left);
        printf("ID: %d | Name: %-15s | Phone: %s\n", root->customer.id, root->customer.name, root->customer.phone);
        displayAllCustomers(root->right);
    }
}

void pushToHistory(char *type, char *issue) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->ticket.ticketId = globalTicketCounter++;
    strcpy(newNode->ticket.type, type);
    strcpy(newNode->ticket.issue, issue);
    newNode->next = completedStackTop;
    completedStackTop = newNode;
}

void viewRecentHistory() {
    if (completedStackTop == NULL) {
        printf(">> History is empty.\n");
        return;
    }
    printf("\n--- Recently Resolved Tickets (Stack) ---\n");
    Node *temp = completedStackTop;
    while (temp != NULL) {
        printf("[ID: %d] Type: %s | Issue: %s\n", temp->ticket.ticketId, temp->ticket.type, temp->ticket.issue);
        temp = temp->next;
    }
}

void addFeedback(int custId, char *msg) {
    FeedbackNode *newNode = (FeedbackNode*)malloc(sizeof(FeedbackNode));
    newNode->customerId = custId;
    strcpy(newNode->message, msg);
    newNode->next = feedbackHead; 
    feedbackHead = newNode;
    printf(">> Feedback received.\n");
}

void viewFeedback() {
    FeedbackNode *temp = feedbackHead;
    printf("\n--- Customer Feedback ---\n");
    while (temp != NULL) {
        printf("Cust ID %d: \"%s\"\n", temp->customerId, temp->message);
        temp = temp->next;
    }
}

void displayServices() {
    printf("\n--- Available Services ---\n");
    for(int i=0; i<7; i++) printf("%s\n", serviceList[i]);
}

/* ====================================================
   INDEX 4: PRIORITY SUB-SYSTEM
   ==================================================== */

void addPriorityTicket() {
    PTNode *newNode = (PTNode*)malloc(sizeof(PTNode));
    int typeChoice, moodChoice;
    printf("Customer ID (String): "); scanf("%s", newNode->custId);
    printf("Name: "); scanf(" %[^\n]s", newNode->name); 
    printf("Issue Type (1.Payment 2.Delivery 3.General): "); scanf("%d", &typeChoice);
    printf("Mood (1.Calm 2.Angry 3.Furious): "); scanf("%d", &moodChoice);

    if (typeChoice == 1) { strcpy(newNode->issueType, "Payment"); newNode->priority = 1; newNode->score = 10; }
    else if (typeChoice == 2) { strcpy(newNode->issueType, "Delivery"); newNode->priority = 2; newNode->score = 10; }
    else { strcpy(newNode->issueType, "General"); newNode->priority = 3; newNode->score = 0; }

    if (moodChoice == 1) strcpy(newNode->mood, "Calm");
    else if (moodChoice == 2) strcpy(newNode->mood, "Angry");
    else strcpy(newNode->mood, "Furious");
    newNode->next = NULL;

    if (newNode->priority == 1) {
        if (highHead == NULL) highHead = highTail = newNode;
        else { highTail->next = newNode; highTail = newNode; }
    } else if (newNode->priority == 2) {
        if (medHead == NULL) medHead = medTail = newNode;
        else { medTail->next = newNode; medTail = newNode; }
    } else {
        if (lowHead == NULL) lowHead = lowTail = newNode;
        else { lowTail->next = newNode; lowTail = newNode; }
    }
    printf("Ticket added.\n");
}

void servePriorityTicket() {
    PTNode *temp;
    if (highHead) { temp=highHead; highHead=highHead->next; }
    else if (medHead) { temp=medHead; medHead=medHead->next; }
    else if (lowHead) { temp=lowHead; lowHead=lowHead->next; }
    else { printf("No tickets.\n"); return; }
    printf("Served: %s\n", temp->name);
    pushToHistory(temp->issueType, temp->mood);
    free(temp);
}

void displayPriorityQueue() {
    PTNode *temp;
    printf("\n[HIGH] "); for(temp=highHead; temp; temp=temp->next) printf("-%s ", temp->name);
    printf("\n[MED] "); for(temp=medHead; temp; temp=temp->next) printf("-%s ", temp->name);
    printf("\n[LOW] "); for(temp=lowHead; temp; temp=temp->next) printf("-%s ", temp->name);
    printf("\n");
}

void runPrioritySubMenu() {
    int sub;
    while(1) {
        printf("\n[Priority Support] 1.Add 2.Serve 3.View 4.Back: ");
        scanf("%d", &sub);
        if(sub==4) return;
        if(sub==1) addPriorityTicket();
        else if(sub==2) servePriorityTicket();
        else if(sub==3) displayPriorityQueue();
    }
}

void serveSmartTicket() {
    if (medHead && strcmp(medHead->mood, "Furious")==0) {
        printf(">> SMART SERVE: De-escalating Furious Customer %s (Medium Queue)\n", medHead->name);
        PTNode *t = medHead; medHead=medHead->next; free(t);
    } else if (lowHead && strcmp(lowHead->mood, "Furious")==0) {
        printf(">> SMART SERVE: De-escalating Furious Customer %s (Low Queue)\n", lowHead->name);
        PTNode *t = lowHead; lowHead=lowHead->next; free(t);
    } else {
        servePriorityTicket();
    }
}

/* ======================================================
   NEW INDEX 5: DEVICE EXCHANGE HUB (DEQUE)
   ====================================================== */

void addTradeIn() {
    TradeNode *newNode = (TradeNode*)malloc(sizeof(TradeNode));
    int condChoice;
    
    printf("Customer ID (Int): "); scanf("%d", &newNode->customerId);
    printf("Device Name: "); scanf(" %[^\n]s", newNode->deviceName);
    printf("Model Year: "); scanf("%d", &newNode->year);
    printf("Condition (1.Mint/New 2.Good 3.Damaged): "); scanf("%d", &condChoice);

    if (condChoice == 1) strcpy(newNode->condition, "Mint");
    else if (condChoice == 2) strcpy(newNode->condition, "Good");
    else strcpy(newNode->condition, "Damaged");

    newNode->next = NULL;
    newNode->prev = NULL;

    // LOGIC: Mint condition or New models go to FRONT (Deque)
    // Damaged/Old go to REAR (Deque)
    if (condChoice == 1 || newNode->year >= 2024) {
        printf(">> High Value Item! Priority Placement.\n");
        // Insert Front
        if (tradeFront == NULL) {
            tradeFront = tradeRear = newNode;
        } else {
            newNode->next = tradeFront;
            tradeFront->prev = newNode;
            tradeFront = newNode;
        }
    } else {
        printf(">> Standard Trade-in. Added to queue.\n");
        // Insert Rear
        if (tradeRear == NULL) {
            tradeFront = tradeRear = newNode;
        } else {
            tradeRear->next = newNode;
            newNode->prev = tradeRear;
            tradeRear = newNode;
        }
    }
}

void processTradeIn() {
    if (tradeFront == NULL) {
        printf(">> No trade-ins pending.\n");
        return;
    }
    TradeNode *temp = tradeFront;
    printf(">> Processing Trade-In: %s (%s, %d)\n", temp->deviceName, temp->condition, temp->year);
    pushToHistory("TradeIn", temp->deviceName);

    if (tradeFront == tradeRear) {
        tradeFront = tradeRear = NULL;
    } else {
        tradeFront = tradeFront->next;
        tradeFront->prev = NULL;
    }
    free(temp);
}

void viewTradeIns() {
    TradeNode *temp = tradeFront;
    printf("\n--- Pending Device Exchanges ---\n");
    if (!temp) printf("(Empty)\n");
    while (temp) {
        printf("[%s] %s - %s\n", (strcmp(temp->condition, "Mint")==0 ? "VIP" : "STD"), temp->deviceName, temp->condition);
        temp = temp->next;
    }
}

void runTradeInSubMenu() {
    int sub;
    while(1) {
        printf("\n=== Device Exchange Hub ===\n");
        printf("1. New Trade-In Request (Deque Insert)\n");
        printf("2. Process Next Trade-In (Deque Pop Front)\n");
        printf("3. View Trade-In Queue\n");
        printf("4. Return to Main Menu\n");
        printf("Choice: ");
        scanf("%d", &sub);

        if(sub==4) return;
        switch(sub) {
            case 1: addTradeIn(); break;
            case 2: processTradeIn(); break;
            case 3: viewTradeIns(); break;
            default: printf("Invalid.\n");
        }
    }
}

/* ====================================================
   NEW INDEX 7: FAST-TRACK VIP (INDEX 5 HELPER)
   ===================================================== */

void fastTrackVIPTradeIn() {
    // This function specifically looks for Mint condition items at the front
    if (tradeFront == NULL) {
        printf(">> Queue is empty.\n");
        return;
    }

    if (strcmp(tradeFront->condition, "Mint") == 0) {
        printf("\n!!! FAST-TRACK APPROVED !!!\n");
        printf("Auto-Processing High Value Item: %s\n", tradeFront->deviceName);
        processTradeIn(); // Re-use process logic
    } else {
        printf("\nX DENIED X\n");
        printf("Next item is '%s' (Condition: %s). Not eligible for Fast-Track.\n", tradeFront->deviceName, tradeFront->condition);
        printf("Please use the standard 'Device Exchange Hub' menu to process this.\n");
    }
}

/* ======================================================
   MAIN DRIVER
   ======================================================*/

int main() {
    int choice, id;
    char name[50], phone[15], buf[100];

    while (1) {
        printf("\n============================================\n");
        printf("    CUSTOMER SERVICE SYSTEM (TECHCARE)      \n");
        printf("============================================\n");
        printf("1. Register New Customer \n");
        printf("2. View All Customers \n");
        printf("3. View Services \n");
        printf("4. Priority Support \n"); 
        printf("5. Device Exchange Hub \n"); // Updated
        printf("6. Smart Serve \n"); 
        printf("7. Fast-Track VIP Trade-Ins \n");        // Updated
        printf("8. View Recent Resolved History \n");
        printf("9. Leave Feedback \n");
        printf("10. View Feedback\n");
        printf("0. Exit\n");
        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter ID: "); scanf("%d", &id);
                printf("Enter Name: "); scanf(" %[^\n]s", name);
                printf("Enter Phone: "); scanf("%s", phone);
                customerRoot = insertCustomer(customerRoot, id, name, phone);
                break;
            case 2: displayAllCustomers(customerRoot); break;
            case 3: displayServices(); break;
            case 4: runPrioritySubMenu(); break;
            
            case 5: // NEW SUB-MENU
                runTradeInSubMenu(); 
                break;

            case 6: serveSmartTicket(); break;

            case 7: // NEW FUNCTION CONNECTED TO INDEX 5
                fastTrackVIPTradeIn(); 
                break;

            case 8: viewRecentHistory(); break;
            case 9:
                printf("Enter Customer ID: "); scanf("%d", &id);
                printf("Enter Feedback: "); scanf(" %[^\n]s", buf); 
                addFeedback(id, buf);
                break;
            case 10: viewFeedback(); break;
            case 0: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}

