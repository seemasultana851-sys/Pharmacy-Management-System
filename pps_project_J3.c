#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEDICINES 100
#define FILENAME "inventory.dat"
#define LOW_STOCK_THRESHOLD 10

// Define structure for a Medicine with Expiry Date fields
struct Medicine {
    int id;
    char name[50];
    int quantity;
    float price;
    int expMonth;
    int expYear;
};

// Function prototypes (fully matched with definitions)
int loadInventory(struct Medicine meds[]);
void saveInventory(struct Medicine meds[], int count);
void viewInventory(struct Medicine meds[], int count);
void addMedicine(struct Medicine meds[], int *count);
void searchMedicine(struct Medicine meds[], int count);
void sellMedicine(struct Medicine meds[], int *count);
void checkAlerts(struct Medicine meds[], int count);

int main() {
    struct Medicine meds[MAX_MEDICINES];
    int count = 0;
    int choice;

    // Load inventory from binary file on startup
    count = loadInventory(meds);

    do {
        printf("\n=========================================\n");
        printf("       PHARMACY MANAGEMENT SYSTEM        \n");
        printf("=========================================\n");
        printf("1. View Inventory\n");
        printf("2. Add New Medicine\n");
        printf("3. Search Medicine\n");
        printf("4. Sell Medicine (Billing)\n");
        printf("5. Check Alerts (Low Stock / Expiry)\n");
        printf("6. Exit & Save\n");
        printf("Enter your choice (1-6): ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Please enter a number.\n");
            while(getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1:
                viewInventory(meds, count);
                break;
            case 2:
                addMedicine(meds, &count);
                break;
            case 3:
                searchMedicine(meds, count);
                break;
            case 4:
                sellMedicine(meds, &count);
                break;
            case 5:
                checkAlerts(meds, count);
                break;
            case 6:
                saveInventory(meds, count);
                printf("\nInventory saved successfully. Exiting program. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice! Please enter a number between 1 and 6.\n");
        }
    } while (choice != 6);

    return 0;
}

// Function to load inventory from binary file
int loadInventory(struct Medicine meds[]) {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("\n[Info] No previous inventory file found. Starting fresh.\n");
        return 0;
    }

    int count = fread(meds, sizeof(struct Medicine), MAX_MEDICINES, file);
    fclose(file);
    printf("\n[Info] Successfully loaded %d medicines from file.\n", count);
    return count;
}

// Function to save inventory to binary file
void saveInventory(struct Medicine meds[], int count) {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("\nError: Could not open file for writing!\n");
        return;
    }

    fwrite(meds, sizeof(struct Medicine), count, file);
    fclose(file);
}

// Function to view all medicines
void viewInventory(struct Medicine meds[], int count) {
    if (count == 0) {
        printf("\nInventory is empty.\n");
        return;
    }

    printf("\n--- INVENTORY LIST ---\n");
    printf("%-5s | %-18s | %-8s | %-8s | %-10s\n", "ID", "Name", "Qty", "Price", "Expiry");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-5d | %-18s | %-8d | $%-7.2f | %02d/%d\n",
               meds[i].id, meds[i].name, meds[i].quantity, meds[i].price,
               meds[i].expMonth, meds[i].expYear);
    }
}

// Function to add a new medicine
void addMedicine(struct Medicine meds[], int *count) {
    if (*count >= MAX_MEDICINES) {
        printf("\nInventory is full! Cannot add more medicines.\n");
        return;
    }

    printf("\nEnter Medicine ID: ");
    scanf("%d", &meds[*count].id);

    printf("Enter Medicine Name: ");
    scanf("%49s", meds[*count].name);

    printf("Enter Quantity: ");
    scanf("%d", &meds[*count].quantity);

    printf("Enter Price per unit: ");
    scanf("%f", &meds[*count].price);

    printf("Enter Expiry Month (1-12): ");
    scanf("%d", &meds[*count].expMonth);

    printf("Enter Expiry Year (e.g., 2028): ");
    scanf("%d", &meds[*count].expYear);

    (*count)++;
    saveInventory(meds, *count);
    printf("\nMedicine added and saved successfully!\n");
}

// Function to search for a medicine by ID
void searchMedicine(struct Medicine meds[], int count) {
    int searchId, found = 0;
    printf("\nEnter Medicine ID to search: ");
    scanf("%d", &searchId);

    for (int i = 0; i < count; i++) {
        if (meds[i].id == searchId) {
            printf("\nMedicine Found:\n");
            printf("ID: %d\nName: %s\nQuantity: %d\nPrice: $%.2f\nExpiry Date: %02d/%d\n",
                   meds[i].id, meds[i].name, meds[i].quantity, meds[i].price,
                   meds[i].expMonth, meds[i].expYear);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("\nMedicine with ID %d not found.\n", searchId);
    }
}

// Function to process a sale
void sellMedicine(struct Medicine meds[], int *count) {
    int sellId, qty, found = 0;
    printf("\nEnter Medicine ID to sell: ");
    scanf("%d", &sellId);

    for (int i = 0; i < *count; i++) {
        if (meds[i].id == sellId) {
            found = 1;
            printf("Enter quantity to sell: ");
            scanf("%d", &qty);

            if (qty <= 0) {
                printf("\nInvalid quantity!\n");
                return;
            }

            if (meds[i].quantity >= qty) {
                meds[i].quantity -= qty;
                float totalAmount = qty * meds[i].price;

                saveInventory(meds, *count);

                printf("\n--- BILL RECEIPT ---\n");
                printf("Medicine: %s\n", meds[i].name);
                printf("Quantity Sold: %d\n", qty);
                printf("Total Amount: $%.2f\n", totalAmount);
                printf("--------------------\n");
                printf("Sale successful! Remaining stock: %d\n", meds[i].quantity);
            } else {
                printf("\nError: Insufficient stock! Only %d units available.\n", meds[i].quantity);
            }
            break;
        }
    }
    if (!found) {
        printf("\nMedicine with ID %d not found.\n", sellId);
    }
}

// Function to check alerts
void checkAlerts(struct Medicine meds[], int count) {
    if (count == 0) {
        printf("\nInventory is empty.\n");
        return;
    }

    int lowStockCount = 0;

    printf("\n=========================================\n");
    printf("            SYSTEM ALERTS & WARNINGS     \n");
    printf("=========================================\n");

    printf("\n[LOW STOCK ALERTS (Quantity < %d)]\n", LOW_STOCK_THRESHOLD);
    for (int i = 0; i < count; i++) {
        if (meds[i].quantity < LOW_STOCK_THRESHOLD) {
            printf(" - ID: %d | Name: %s | Current Stock: %d (Needs Restock!)\n",
                   meds[i].id, meds[i].name, meds[i].quantity);
            lowStockCount++;
        }
    }
    if (lowStockCount == 0) {
        printf(" -> All items have sufficient stock levels.\n");
    }

    printf("\n[EXPIRY OVERVIEW]\n");
    for (int i = 0; i < count; i++) {
        printf(" - ID: %d | Name: %s | Expires: %02d/%d\n",
               meds[i].id, meds[i].name, meds[i].expMonth, meds[i].expYear);
    }
    printf("-----------------------------------------\n");
}
