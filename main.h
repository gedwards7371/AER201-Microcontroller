// Main execution state enum plus main function and an output-stopping function
enum states {UI_state, Sorting_state, DoneSorting_state, Testing_state, EmergencyStop_state};
enum states machine_state;
void main(void);
void stopSignals(void);

// Other functions that it's not worth making individual headers for
void machineConfig(void);
void PortTestA5(void);

// Debug flag
extern int debug;