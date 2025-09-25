/*
evk mother board
EVK_MB_05c - initial release
ADF4108 CLI light (2 command)
ADF4108 write Reg

EVK_MB_06
fix array array_set_reg_gpio:
was
AD_4_IO5_Vread,
AD_4_IO5_Vread,
AD_4_IO5_Vread,
becomes
AD_4_IO5_Vread,
AD_4_IO6_Vread,
AD_4_IO7_Vread,
*/
#include <EEPROM.h>
#include <SPI.h>

const char *FW = "FW_10000-7";
const char *prompt = ">";
String command = "";
byte c = 0x00;

//int sn = ((EEPROM.read(1) & 0XFFFF) << 8) | (EEPROM.read(0) & 0xFFFF);
byte sn = EEPROM.read(0);
//int sn_db;

uint32_t wReg;
bool valid;
bool boot_ok = false;

//M95320 - 25LC320: EEPROM 4 Kbyte, 32 byte page size
#define EEPROM_WRITE_ENABLE  6
#define EEPROM_WRITE_DISABLE  4
#define EEPROM_READ_STATUS_REGISTER  5
#define EEPROM_WRITE_STATUS_REGISTER  1
#define EEPROM_READ_MEMORY_ARRAY  3
#define EEPROM_WRITE_MEMORY_ARRAY 2
#define page_size 32

//**********assigning pin names to variables******************
//atmega 32U4
#define CE_AD_DTX 0
#define CE_AD_1  1
#define CE_AD_2  4
#define CE_AD_3  5
#define CE_AD_4  6
#define CE_AD_5  7
#define CE_AD_6  8
#define VD1_EN  9
#define VD2_EN  10
#define VCO_EN  11
#define e2prom_CE  12
#define LE_ADF4108  13
#define ID_VCO A0       // pin 36
#define VD3_EN  A1      // pin 37
#define VD4_EN  A2      // pin 38
#define VD5_EN  A3      // pin 39
#define EN_ISOLATOR  A4 // pin 40

//AD5592_1
#define Vref_VD1  0
#define Vref_VD2  1
#define Vref_VD3  2
#define Vref_VD4  3
#define Vref_VD5  4
#define ID3_read  32
#define ID4_read  64
#define ID5_read  128

//AD5592_2
#define VD5_read  1
#define VD4_read  2
#define VD3_read  4
#define VD2_read  8
#define VD1_read  16
#define AD_2_IO5_Vread  32
#define AD_2_IO6_Vwrite  6

//AD5592_3
#define AD_3_IO0_Vwrite  0
#define AD_3_IO1_Vwrite  1
#define AD_3_IO2_Vwrite  2
#define AD_3_IO3_Vwrite  3
#define AD_3_IO4_Vwrite  4
#define AD_3_IO5_Vwrite  5
#define AD_3_IO6_Vwrite  6
#define AD_3_IO7_Vwrite  7

//AD5592_4
#define AD_4_IO0_Vwrite  0
#define AD_4_IO1_Vwrite  1
#define AD_4_IO2_Vwrite  2
#define AD_4_IO3_Vwrite  3
#define PLL_LOCK  16
#define AD_4_IO5_Vread  32
#define AD_4_IO6_Vread  64
#define AD_4_IO7_Vread  128

//AD5592_5
#define AD_5_IO0_Vread  1
#define AD_5_IO1_Vread  2
#define AD_5_IO2_Vread  4
#define AD_5_IO3_Vread  8
#define AD_5_IO4_Vread  16
#define AD_5_IO5_Vread  32
#define AD_5_IO6_Vread  64
#define AD_5_IO7_Vread  128

//AD5592_6
#define Cable1_pin2  0
#define Cable1_pin1  1
#define Cable2_pin1  2
#define Cable2_pin2  3
#define DB_code  16

#define mV_EN  6
#define m3V_EN  7

//AD5592_ DTX
#define Vref_PAI_cloop  0
#define Vref_PAII_cloop  1
#define Vref_PAIII_cloop  2
#define Vref_PAIV_cloop  3
#define Vg_PAI_fwd  4
#define Vg_PAII_fwd  5
#define Vg_PAIII_fwd  6
#define Vg_PAIV_fwd  7

//***********define & initialize variables********
uint16_t ADC0;
uint16_t DAC0;
byte GPIO;
byte D15D8;
byte D7D0;
bool DAC_readback = false;
#define Vref_32u4 3.3
#define Vref_AD5592 2.5
#define Vmin_AD5592 0.0
#define Vmax_AD5592 2.49

float VD_min;
float VD_max;
float VgGAAS_min;
float Vc_SIGE_max;
float Vif_min;
float Vif_max;
float Vref_cloop_min;
float Vref_cloop_max;
float Vg_pa_min;
float Vg_pa_max;

#define array_lenght 48
byte array_set_reg_CE[array_lenght] = {
  CE_AD_1,
  CE_AD_1,
  CE_AD_1,
  CE_AD_1,
  CE_AD_1,
  CE_AD_1,
  CE_AD_1,
  CE_AD_1,
  CE_AD_2,
  CE_AD_2,
  CE_AD_2,
  CE_AD_2,
  CE_AD_2,
  CE_AD_2,
  CE_AD_2,
  CE_AD_2,
  CE_AD_3,
  CE_AD_3,
  CE_AD_3,
  CE_AD_3,
  CE_AD_3,
  CE_AD_3,
  CE_AD_3,
  CE_AD_3,
  CE_AD_4,
  CE_AD_4,
  CE_AD_4,
  CE_AD_4,
  CE_AD_4,
  CE_AD_4,
  CE_AD_4,
  CE_AD_4,
  CE_AD_5,
  CE_AD_5,
  CE_AD_5,
  CE_AD_5,
  CE_AD_5,
  CE_AD_5,
  CE_AD_5,
  CE_AD_5,
  CE_AD_DTX,
  CE_AD_DTX,
  CE_AD_DTX,
  CE_AD_DTX,
  CE_AD_DTX,
  CE_AD_DTX,
  CE_AD_DTX,
  CE_AD_DTX };

byte array_set_reg_gpio[array_lenght] = {
  Vref_VD1,
  Vref_VD2,
  Vref_VD3,
  Vref_VD4,
  Vref_VD5,
  ID3_read,
  ID4_read,
  ID5_read,
  VD5_read,
  VD4_read,
  VD3_read,
  VD2_read,
  VD1_read,
  AD_2_IO5_Vread,
  AD_2_IO6_Vwrite,
  7,
  AD_3_IO0_Vwrite,
  AD_3_IO1_Vwrite,
  AD_3_IO2_Vwrite,
  AD_3_IO3_Vwrite,
  AD_3_IO4_Vwrite,
  AD_3_IO5_Vwrite,
  AD_3_IO6_Vwrite,
  AD_3_IO7_Vwrite,
  AD_4_IO0_Vwrite,
  AD_4_IO1_Vwrite,
  AD_4_IO2_Vwrite,
  AD_4_IO3_Vwrite,
  PLL_LOCK,
  AD_4_IO5_Vread,
  AD_4_IO6_Vread,
  AD_4_IO7_Vread,
  AD_5_IO0_Vread,
  AD_5_IO1_Vread,
  AD_5_IO2_Vread,
  AD_5_IO3_Vread,
  AD_5_IO4_Vread,
  AD_5_IO5_Vread,
  AD_5_IO6_Vread,
  AD_5_IO7_Vread,
  Vref_PAI_cloop,
  Vref_PAII_cloop,
  Vref_PAIII_cloop,
  Vref_PAIV_cloop,
  Vg_PAI_fwd,
  Vg_PAII_fwd,
  Vg_PAIII_fwd,
  Vg_PAIV_fwd };

byte array_calc[array_lenght] = {
  1,
  1,
  1,
  1,
  1,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  5,
  2,
  0,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  3,
  2,
  2,
  2,
  2,
  4,
  4,
  4,
  4 };

byte array_dis_isolator[array_lenght] = {
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0 };

#define add_str_ofs 308
#define add_str_k 312

#define add_str_ofs_M 628
#define add_str_k_M 632

double array_set_reg_LL[array_lenght];
double array_set_reg_LH[array_lenght];

#define array_vd_en_lenght 6
byte array_vd_en[array_vd_en_lenght] = {VD1_EN, VD2_EN, VD3_EN, VD4_EN, VD5_EN, VCO_EN};
String cmd_vd[array_vd_en_lenght] = {"vd1", "vd2", "vd3", "vd4", "vd5", "vco"};

bool found = false;

//***********define & initialize variables for ADF4108********
double fvco;// MHz
uint8_t ADF4108_REF = EEPROM.read(19);
uint16_t R = ((EEPROM.read(21) & 0XFFFF) << 8) | (EEPROM.read(20) & 0xFFFF);

double ADF4108_PFD;

uint8_t ADF4108_PRES = EEPROM.read(22);
uint8_t PRESCALER_array [] = {8, 16 ,32 ,64};
uint8_t PRESCALER;

uint8_t ADF4108_COUNTER_RES = EEPROM.read(23);
uint8_t ADF4108_MUX_OUT = EEPROM.read(24);
uint8_t ADF4108_PHASE_DET_POL = EEPROM.read(25);
uint8_t ADF4108_CRG_PUMP_3ST = EEPROM.read(26);
uint8_t ADF4108_FAST_LOCK = EEPROM.read(27);
uint8_t ADF4108_TIMEOUT = EEPROM.read(28);
uint8_t ADF4108_CRG_PUMP_SET1 = EEPROM.read(29);
uint8_t ADF4108_CRG_PUMP_SET2 = EEPROM.read(30);
uint8_t ADF4108_PWR_DOWN = EEPROM.read(31);
uint8_t ADF4108_ABPW = EEPROM.read(32);
uint8_t ADF4108_LOCK_DET_PRECISION = EEPROM.read(33);
uint8_t ADF4108_CRG_PUMP_GAIN = EEPROM.read(34);

uint16_t N;
uint16_t A;
uint16_t B;

uint32_t REF_Reg = 0b00;
uint32_t AB_Reg = 0b01;
uint32_t FUNCT_Reg = 0b10;
uint32_t INIT_Reg = 0b11;

int start_add_page;

union{
  byte array[4];
  double num;
} doub2arr;

//#define COMMAND_READ_AD5592_REGISTER

//*************************void setup*************************************
void setup() {
//  while (!Serial) {}
  Serial.begin(115200);
  delay (2000);
  Serial.println(F("EVK - Mother Board v1.0"));
  Serial.print(F("Firmware Version = "));
  Serial.println (FW);
  Serial.print(F("Serial Number = "));
  Serial.println(sn);

//  analogReference(EXTERNAL);
  
//  pinMode (CE_AD_DTX, OUTPUT);
//  digitalWrite (CE_AD_DTX, HIGH);
//  pinMode (CE_AD_1, OUTPUT);
//  digitalWrite (CE_AD_1, HIGH);
//  pinMode (CE_AD_2, OUTPUT);
//  digitalWrite (CE_AD_2, HIGH);
//  pinMode (CE_AD_3, OUTPUT);
//  digitalWrite (CE_AD_3, HIGH);
//  pinMode (CE_AD_4, OUTPUT);
//  digitalWrite (CE_AD_4, HIGH);
//  pinMode (CE_AD_5, OUTPUT);
//  digitalWrite (CE_AD_5, HIGH);
//  pinMode (CE_AD_6, OUTPUT);
//  digitalWrite (CE_AD_6, HIGH);
//  pinMode (e2prom_CE, OUTPUT);
//  digitalWrite (e2prom_CE, HIGH);
//  pinMode (LE_ADF4108, OUTPUT);
//  digitalWrite (LE_ADF4108, HIGH);
//
//  pinMode (VD1_EN, OUTPUT);
//  digitalWrite (VD1_EN, LOW);
//  pinMode (VD2_EN, OUTPUT);
//  digitalWrite (VD2_EN, LOW);
//  pinMode (VD3_EN, OUTPUT);
//  digitalWrite (VD3_EN, LOW);
//  pinMode (VD4_EN, OUTPUT);
//  digitalWrite (VD4_EN, LOW);
//  pinMode (VD5_EN, OUTPUT);
//  digitalWrite (VD5_EN, LOW);
//  pinMode (VCO_EN, OUTPUT);
//  digitalWrite (VCO_EN, LOW);
//  pinMode (EN_ISOLATOR, OUTPUT);
//  digitalWrite (EN_ISOLATOR, HIGH);

  // --- Set chip select and control pins to OUTPUT and HIGH ---
  byte highPins[] = {
    CE_AD_DTX, CE_AD_1, CE_AD_2, CE_AD_3, CE_AD_4, CE_AD_5, CE_AD_6,
    e2prom_CE, LE_ADF4108, EN_ISOLATOR
  };
  
  for (byte i = 0; i < sizeof(highPins); i++) {
    pinMode(highPins[i], OUTPUT);
    digitalWrite(highPins[i], HIGH);
  }
  
  // --- Set voltage enable pins to OUTPUT and LOW ---
  byte lowPins[] = {
    VD1_EN, VD2_EN, VD3_EN, VD4_EN, VD5_EN, VCO_EN
  };
  
  for (byte i = 0; i < sizeof(lowPins); i++) {
    pinMode(lowPins[i], OUTPUT);
    digitalWrite(lowPins[i], LOW);
  }


  SPI.begin();
//  set SPI for AD5592R
//  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  
  conf_AD5592_1 (CE_AD_1);
  conf_AD5592_2 (CE_AD_2);
  conf_AD5592_3 (CE_AD_3);
  conf_AD5592_4 (CE_AD_4);
  conf_AD5592_5 (CE_AD_5);
  conf_AD5592_6 (CE_AD_6);
  conf_AD5592_DTX (CE_AD_DTX);

  // Retrieve fvco value from EEPROM address 10
  EEPROM.get(10, fvco);
  
  // Validate the value: if it's not a number or out of expected range, reset to default
  if (isnan(fvco) || fvco < 1000 || fvco > 8000) {
    fvco = 6500; // Default value
    EEPROM.put(10, fvco); // Save corrected value back to EEPROM
  }
  
  // Read ADF4108 reference frequency from EEPROM address 19
  EEPROM.get(19, ADF4108_REF);
  
  // Validate the value: must be between 1 and 100 MHz
  if (ADF4108_REF < 1 || ADF4108_REF > 100) {
    ADF4108_REF = 25; // Default value
    EEPROM.write(19, ADF4108_REF); // Save corrected value
  }

  // Validate R value: must be between 1 and 16383
  if (R < 1 || R > 16383) {
    R = 48; // Default value
  
    // Store R as two bytes in EEPROM:
    // High byte at address 21, low byte at address 20
    EEPROM.write(21, (R >> 8) & 0xFF); // High byte
    EEPROM.write(20, R & 0xFF);        // Low byte
  }

  // Validate ADF4108_PRES: must be 0, 1, 2, or 3
  if (ADF4108_PRES > 3) {
    ADF4108_PRES = 2; // Default value
    EEPROM.write(22, ADF4108_PRES); // Save corrected value to EEPROM
  }
  
  // Set the prescaler value from the lookup array
  PRESCALER = PRESCALER_array[ADF4108_PRES];
 
  if (ADF4108_COUNTER_RES > 1) {ADF4108_COUNTER_RES = 0; EEPROM.write(23, ADF4108_COUNTER_RES);}
//  Serial.print(F("ADF4108_COUNTER_RES = "));
//  Serial.println(ADF4108_COUNTER_RES);

  if (ADF4108_MUX_OUT > 7) {ADF4108_MUX_OUT = 1; EEPROM.write(24, ADF4108_MUX_OUT);}
//  Serial.print(F("ADF4108_MUX_OUT = "));
//  Serial.println(ADF4108_MUX_OUT);

  if (ADF4108_PHASE_DET_POL > 1) {ADF4108_PHASE_DET_POL = 1; EEPROM.write(25, ADF4108_PHASE_DET_POL);}
//  Serial.print(F("ADF4108_PHASE_DET_POL = "));
//  Serial.println(ADF4108_PHASE_DET_POL);

  if (ADF4108_CRG_PUMP_3ST > 1) {ADF4108_CRG_PUMP_3ST = 0; EEPROM.write(26, ADF4108_CRG_PUMP_3ST);}
//  Serial.print(F("ADF4108_CRG_PUMP_3ST = "));
//  Serial.println(ADF4108_CRG_PUMP_3ST);

  if (ADF4108_FAST_LOCK > 3) {ADF4108_FAST_LOCK = 0; EEPROM.write(27, ADF4108_FAST_LOCK);}
//  Serial.print(F("ADF4108_FAST_LOCK = "));
//  Serial.println(ADF4108_FAST_LOCK);

  if (ADF4108_TIMEOUT > 15) {ADF4108_TIMEOUT = 0; EEPROM.write(28, ADF4108_TIMEOUT);}
//  Serial.print(F("ADF4108_TIMEOUT = "));
//  Serial.println(ADF4108_TIMEOUT);

  if (ADF4108_CRG_PUMP_SET1 > 7) {ADF4108_CRG_PUMP_SET1 = 3; EEPROM.write(29, ADF4108_CRG_PUMP_SET1);}
//  Serial.print(F("ADF4108_CRG_PUMP_SET1 = "));
//  Serial.println(ADF4108_CRG_PUMP_SET1);

  if (ADF4108_CRG_PUMP_SET2 > 7) {ADF4108_CRG_PUMP_SET2 = 3; EEPROM.write(30, ADF4108_CRG_PUMP_SET2);}
//  Serial.print(F("ADF4108_CRG_PUMP_SET2 = "));
//  Serial.println(ADF4108_CRG_PUMP_SET2);

  if (ADF4108_PWR_DOWN > 3) {ADF4108_PWR_DOWN = 0; EEPROM.write(31, ADF4108_PWR_DOWN);}
//  Serial.print(F("ADF4108_PWR_DOWN = "));
//  Serial.println(ADF4108_PWR_DOWN);

  if (ADF4108_ABPW > 3) {ADF4108_ABPW = 0; EEPROM.write(32, ADF4108_ABPW);}
//  Serial.print(F("ADF4108_ABPW = "));
//  Serial.println(ADF4108_ABPW);

  if (ADF4108_LOCK_DET_PRECISION > 1) {ADF4108_LOCK_DET_PRECISION = 0; EEPROM.write(33, ADF4108_LOCK_DET_PRECISION);}
//  Serial.print(F("ADF4108_LOCK_DET_PRECISION = "));
//  Serial.println(ADF4108_LOCK_DET_PRECISION);

  if (ADF4108_CRG_PUMP_GAIN > 1) {ADF4108_CRG_PUMP_GAIN = 0; EEPROM.write(34, ADF4108_CRG_PUMP_GAIN);}
//  Serial.print(F("ADF4108_CRG_PUMP_GAIN = "));
//  Serial.println(ADF4108_CRG_PUMP_GAIN);

  init_LL_LH();
  
  byte seq_lenght = 26;
  int seq [seq_lenght] = {0,1,2,3,4,14,16,17,18,19,20,21,22,23,24,25,26,27,40,41,42,43,44,45,46,47};
//  String seq_label [seq_lenght] = {
//    "vd1",
//    "vd2",
//    "vd3",
//    "vd4",
//    "vd5",
//    "ad2.6",
//    "ad3.0",
//    "ad3.1",
//    "ad3.2",
//    "ad3.3",
//    "ad3.4",
//    "ad3.5",
//    "ad3.6",
//    "ad3.7",
//    "ad4.0",
//    "ad4.1",
//    "ad4.2",
//    "ad4.3",
//    "dtx.0",
//    "dtx.1",
//    "dtx.2",
//    "dtx.3",
//    "dtx.4",
//    "dtx.5",
//    "dtx.6",
//    "dtx.7" };
  
  Serial.println();
  Serial.println(F("*********************"));
  
  // Read and display the status of -V_EN and 3.3_EN control signals
  byte status = DR_AD5592(CE_AD_6); // Read once to avoid multiple SPI calls
  
  Serial.print(F("-V_EN = "));
  Serial.println(bitRead(status, m3V_EN)); // Read bit for -V_EN
  
  Serial.print(F("-3.3_EN = "));
  Serial.println(bitRead(status, mV_EN));  // Read bit for 3.3_EN
  
  // Check if both power enable signals are active
  boot_ok = (bitRead(status, m3V_EN) && bitRead(status, mV_EN));
  
  if (boot_ok) {
    // Read the raw data from the AD5592 and extract cable status bits
    byte raw_data = DR_AD5592(CE_AD_6);
  
    // Extract status bits for cable 1 (bits 0 and 1)
    byte cbl_1 = raw_data & B00000011;
  
    // Extract status bits for cable 2 (bits 2 and 3)
    byte cbl_2 = (raw_data >> 2) & B00000011;
  
    // Evaluate cable 1 status
    Serial.print(F("cable_1 "));    
    switch (cbl_1) {
      case 0:
        Serial.println(F("general error"));
        break;
      case 1:
        Serial.println(F("error! wrong position"));
        break;
      case 2:
        Serial.println(F("OK"));
        break;
      default:
        Serial.println(F("error! check if it is inserted properly"));
        break;
    }
  
    // Evaluate cable 2 status
    Serial.print(F("cable_2 "));
    switch (cbl_2) {
      case 0:
        Serial.println(F("general error"));
        break;
      case 1:
        Serial.println(F("error! wrong position"));
        break;
      case 2:
        Serial.println(F("OK"));
        break;
      default:
        Serial.println(F("error! check if it is inserted properly"));
        break;
    }

    // Final check: both cables must be OK to proceed
    boot_ok = (cbl_1 == 2 && cbl_2 == 2);
  }

  

  if (boot_ok) {
    // Read the DB code using the AR_AD5592 function
    double db_code = AR_AD5592(CE_AD_6, DB_code, 0, 1);
  
    // Check the value range of db_code and print corresponding message
    if (db_code > 0.1 && db_code < 0.3) {
      Serial.println(F("DB => Sip-eBand_TX")); // TX configuration detected
    }
    else if (db_code > 0.31 && db_code < 0.5) {
      Serial.println(F("DB => Sip-eBand_RX")); // RX configuration detected
    }
    else {
      Serial.println(F("DB type not recognized")); // Unknown configuration
      boot_ok = false; // Mark boot as failed
    }
  }
  
  if (boot_ok) {
    Serial.println(F("*********************"));
    Serial.println();
    Serial.println(F("Start setting VDAC defaults"));
    Serial.print(prompt);
  
    for (int a = 0; a < seq_lenght; a++) {
      int i = seq[a];
      double vout;
  
      // Read VOUT value from EEPROM (each value is 4 bytes apart)
      EEPROM.get(140 + (a * 4), vout);
  
      // Check if VOUT is within allowed limits
      if (vout >= array_set_reg_LL[i] && vout <= array_set_reg_LH[i]) {
        double ofs = read_ofs(i);           // Read offset
        double k = read_k(i);               // Read gain factor
        double vda = 0;                     // Calculated DAC value
        byte calc = array_calc[i];         // Calculation method
        bool dis_isolator = array_dis_isolator[i]; // Whether to disable isolator
  
        // Apply the correct formula based on calculation mode
        if (calc == 1) vda = (ofs - vout) / k;
        else if (calc == 2) vda = (vout / k) + ofs;
        else if (calc == 4) vda = (vout + ofs) * k;
  
        // Check if calculated VDAC is within DAC limits
        if (vda >= Vmin_AD5592 && vda <= Vmax_AD5592) {
          if (dis_isolator) digitalWrite(EN_ISOLATOR, LOW); // Disable isolator if needed
  
          // Write to DAC and verify with readback
          AW_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], vda);
          if (DAC_readback) {
            boot_ok = true;
//            print_ok();
          } else {
            boot_ok = false;
            print_error_DAC_readback();
          }
  
          if (dis_isolator) digitalWrite(EN_ISOLATOR, HIGH); // Re-enable isolator
        } else {
          boot_ok = false;
          print_vdac_exc(); // VDAC out of range
        }
      } else {
        boot_ok = false;
        print_exc_limit(); // VOUT out of allowed limits
      }
    }
  }
  
  if (boot_ok){
    prompt = "siae>";
    Serial.println("type help to see command list");
    Serial.print(prompt);
  }
  else {
    Serial.println();
    prompt = "error>";
    Serial.print(prompt); 
  }
}


//*************************void loop*************************************
void loop() {
  if (Serial.available()){
    c = Serial.read();
    if (c == 0x0D){
      Serial.println();

      // Handle basic serial commands
      if (command == "help") {
        print_cmdlist();           // Print list of available commands
        Serial.print(prompt);      // Show prompt again
      }
//      else if (command == "error") {
//        print_error_code_list();   // Print list of error codes
//        Serial.print(prompt);      // Show prompt again
//      }
      else if (command == "F") {
        Serial.println(FW);        // Print firmware version
        print_ok();                // Confirm success
      }
      else if (command == "S") {
        Serial.println(sn);        // Print serial number
        print_ok();                // Confirm success
      }
      else if (command.charAt(0) == 'S'){
        // Extract the number after 'S ' (assuming format like "S 123")
        int new_sn = command.substring(2).toInt();
      
        // Validate serial number range
        if (new_sn < 255) {
          sn = new_sn;                      // Update serial number
          EEPROM.write(0, sn);             // Save to EEPROM at address 0
          Serial.println(sn);              // Print the new serial number
          print_ok();                      // Confirm success
        } else {
          print_exc_limit();               // Print error if out of range
        }
      }
      //      else if (command.charAt(0) == 'G' && command.charAt(1) == 'E'){
      //        if ((((command.substring(3).toInt()) < 1024) && ((command.substring(3).toInt()) > 99)) || ((command.substring(3).toInt()) == 10) ){
      //          double value;
      //          EEPROM.get((command.substring(3).toInt()), value);
      //          if (isnan(value)){Serial.println("eeprom data isn't a number"); print_isnt_num();}
      //          else {Serial.println(value, 3); print_ok();}
      //          
      //        }
      //        else print_exc_limit();
      //      }
      // Handle command starting with "GE" to read a double value from EEPROM
      else if (command.charAt(0) == 'G' && command.charAt(1) == 'E') {
        // Extract the EEPROM address from the command string (after "GE ")
        int ADD = command.substring(3).toInt();
      
        // Validate address: must be between 100 and 1023, or exactly 10
        if ((ADD > 99 && ADD < 1024) || ADD == 10) {
          double value;
          EEPROM.get(ADD, value); // Read double value from EEPROM
      
          // Check if the value is a valid number
          if (isnan(value)) {
            Serial.println("EEPROM data isn't a number");
            print_isnt_num(); // Custom error handler
          } else {
            Serial.println(value, 3); // Print value with 3 decimal places
            print_ok();               // Confirm success
          }
        } else {
          print_exc_limit(); // Print error if address is out of valid range
        }
      }      
//      else if (command.charAt(0) == 'P' && command.charAt(1) == 'E'){
//        byte pos = command.indexOf(',');
//        int ADD = command.substring(3, pos).toInt();
//        double value = command.substring(pos + 1).toDouble();
//        if (((ADD < 1024 && ADD > 99) || ADD == 10) && value > -1000 && value < 10000){
//          EEPROM.put(ADD, value);
//          if (ADD < 137 && ADD > 99){init_LL_LH();}
//          print_ok();
//        }
//        else print_exc_limit();
//      }
      // Handle command starting with "PE" to write a double value to EEPROM
      else if (command.charAt(0) == 'P' && command.charAt(1) == 'E') {
        // Find the comma position separating address and value
        byte pos = command.indexOf(',');
      
        // Extract EEPROM address and value from the command string
        int ADD = command.substring(3, pos).toInt();
        double value = command.substring(pos + 1).toDouble();
      
        // Validate address and value ranges
        if (((ADD > 99 && ADD < 1024) || ADD == 10) && value > -1000 && value < 10000) {
          EEPROM.put(ADD, value); // Write the double value to EEPROM
      
          // If the address is within the LL/LH config range, reinitialize limits
          if (ADD > 99 && ADD < 137) {
            init_LL_LH();
          }
      
          print_ok(); // Confirm success
        } else {
          print_exc_limit(); // Print error if address or value is out of range
        }
      }

      //      else if (command.charAt(0) == 'R' && command.charAt(1) == 'E'){
      //          if ((command.substring(3).toInt()) < 1024){
      //
      //            Serial.println(EEPROM.read(command.substring(2).toInt()));
      //            print_ok();
      //          }
      //          else print_exc_limit();
      //      }
      // Check if the command starts with 'R' and 'E' (Read EEPROM)
      else if (command.charAt(0) == 'R' && command.charAt(1) == 'E') {
      
          // Convert the substring starting from index 3 to an integer
          // This is used to validate the EEPROM address
          int address_check = command.substring(3).toInt();
      
          // Ensure the address is within EEPROM bounds (0–1023)
          if (address_check < 1024) {
      
              // Convert the substring starting from index 2 to an integer
              // This is the actual address used to read from EEPROM
              int address = command.substring(2).toInt();
      
              // Read the value from EEPROM and print it to the Serial Monitor
              Serial.println(EEPROM.read(address));
      
              // Indicate successful operation
              print_ok();
          } else {
              // Address is out of bounds, print error
              print_exc_limit();
          }
      }

      //      else if (command.charAt(0) == 'W' && command.charAt(1) == 'E'){
      //        byte pos = command.indexOf(',');
      //        int ADD = command.substring(3, pos).toInt();
      //        byte value = command.substring(pos + 1).toInt();
      //        if (ADD < 1024 && value < 256){
      //          EEPROM.write(ADD, value);
      ////          Serial.print("eeprom write byte[");
      ////          Serial.print(ADD);
      ////          Serial.print("] = ");
      ////          Serial.println(value);
      //          print_ok();
      //        }
      //        else print_exc_limit();
      //      }
      // Check if the command starts with 'W' and 'E' (Write EEPROM)
      else if (command.charAt(0) == 'W' && command.charAt(1) == 'E') {
      
          // Find the position of the comma separating address and value
          byte pos = command.indexOf(',');
      
          // Extract EEPROM address from the command (between index 3 and comma)
          int address = command.substring(3, pos).toInt();
      
          // Extract value to write (after the comma)
          byte value = command.substring(pos + 1).toInt();
      
          // Validate address and value ranges
          if (address < 1024 && value < 256) {
              // Write the value to EEPROM at the specified address
              EEPROM.write(address, value);
      
              // Indicate successful operation
              print_ok();
          } else {
              // Address or value out of bounds, print error
              print_exc_limit();
          }
      }

      else if (command.charAt(0) == 'G' && command.charAt(1) == 'M'){
        if ((command.substring(3).toInt()) < 32000 && (command.substring(3).toInt()) > 99){
        int ADD = command.substring(3).toInt();
//          Serial.print("ext eeprom get double [");
//          Serial.print(ADD);
//          Serial.print("] = ");
          Serial.println(e2prom_get(ADD),3);
          print_ok();
        }
        else print_exc_limit();
      }
      else if (command.charAt(0) == 'P' && command.charAt(1) == 'M'){
        byte pos = command.indexOf(',');
        int ADD = command.substring(3, pos).toInt();
        double value = command.substring(pos + 1).toDouble();
        if (ADD < 32000 && ADD > 99 && value > -1000 && value < 1000){
          e2prom_put (ADD, value);
//          Serial.print("ext eeprom put double[");
//          Serial.print(ADD);
//          Serial.print("] = ");
//          Serial.println(value, 3);
          print_ok();
        }
        else print_exc_limit();
      }

//      else if (command == "VD_min"){Serial.println(VD_min); print_ok();}
//      else if (command == "VD_max"){Serial.println(VD_max); print_ok();}
//      else if (command == "VgGAAS_min"){Serial.println(VgGAAS_min); print_ok();}
//      else if (command == "Vc_SIGE_max"){Serial.println(Vc_SIGE_max); print_ok();}
//      else if (command == "Vif_min"){Serial.println(Vif_min); print_ok();}
//      else if (command == "Vif_max"){Serial.println(Vif_max); print_ok();}
//      else if (command == "Vref_cloop_min"){ Serial.println(Vref_cloop_min); print_ok();}
//      else if (command == "Vref_cloop_max"){Serial.println(Vref_cloop_max); print_ok();}
//      else if (command == "Vg_pa_min"){Serial.println(Vg_pa_min); print_ok();}
//      else if (command == "Vg_pa_max"){Serial.println(Vg_pa_max); print_ok();}

//      else if (command == "cbl1.2"){Serial.println(bitRead(DR_AD5592(CE_AD_6), Cable1_pin2)); print_ok();}
//      else if (command == "cbl1.1"){Serial.println(bitRead(DR_AD5592(CE_AD_6), Cable1_pin1)); print_ok();}
//      else if (command == "cbl2.1"){Serial.println(bitRead(DR_AD5592(CE_AD_6), Cable2_pin1)); print_ok();}
//      else if (command == "cbl2.2"){Serial.println(bitRead(DR_AD5592(CE_AD_6), Cable2_pin2)); print_ok();}
      // Check if the command is "db.type"
      else if (command == "db.type") {
      
          // Read the db_code value using AR_AD5592 function
          // Parameters: CE_AD_6 (chip enable), DB_code (channel), 0 (dummy), 1 (read mode)
          double db_code = AR_AD5592(CE_AD_6, DB_code, 0, 1);
      
          // Identify the type based on db_code range
          if (db_code > 0.1 && db_code < 0.3) {
              Serial.println(F("Sip-eBand_TX"));
          }
//          else if (db_code > 0.31 && db_code < 0.5) {
//              Serial.println(F("Sip-eBand_RX"));
//          }
      
          // Indicate successful operation
          print_ok();
      }
      // Check if the command is "idvco"
      else if (command == "idvco") {
      
          // --- Read and validate 'ofs' (offset) from EEPROM address 300 ---
          double ofs = 0;
          EEPROM.get(300, ofs);
      
          // If 'ofs' is NaN or out of acceptable range, reset to 0 and store it
          if (isnan(ofs) || ofs < -1000 || ofs > 1000) {
              ofs = 0;
              EEPROM.put(300, ofs);
          }
      
          // --- Read and validate 'k' (coefficient) from EEPROM address 304 ---
          double k = 0;
          EEPROM.get(304, k);
      
          // If 'k' is NaN or out of acceptable range, reset to 1 and store it
          if (isnan(k) || k < -1000 || k > 1000) {
              k = 1;
              EEPROM.put(304, k);
          }
      
          // --- Compute and print the result using AR_32u4 with validated values ---
          Serial.println(AR_32u4(ID_VCO, ofs, k), 3);
      
          // Indicate successful operation
          print_ok();
      }

//*************************out command*************************************
//      else if (command.substring(0, 3) == "out"){
//        if (command.substring(7, 4) == "get"){         
//          String vdx = command.substring(11, 8);
//          bool found = false;
//          int i = 0;
//          while (i < array_vd_en_lenght & !found) {
//            if (vdx == cmd_vd[i]){
//              Serial.println(digitalRead(array_vd_en[i]));
//              found = true;
//            }
//            else i++;
//          }
//          if (found){print_ok();} else print_cmd_nfound();
//        }
//        else if (command.substring(7, 4) == "set"){         
//         if (is_numeric(command.substring(12))){
//           if ((command.substring(12).toInt()) < 2) {
//              String vdx = command.substring(11, 8);
//              bool found = false;
//              int i = 0;
//              while (i < array_vd_en_lenght & !found) {
//                if (vdx == cmd_vd[i]){
//                  DW_32u4 (array_vd_en[i], command.substring(12).toInt());
//                  delay(200);
//                  if ((vdx == "vco") && (bool)command.substring(12).toInt()){
//                    reg_calc();
//                    write_adf4108(INIT_Reg);
//                    write_adf4108(REF_Reg);
//                    write_adf4108(AB_Reg);
//                  }
//                  found = true;
//                }
//                else i++;
//              }
//              if (found){print_ok();} else print_cmd_nfound();
//            }       
//            else print_exc_limit();
//          }
//          else {print_isnt_num();}
//        }
//        else print_cmd_nfound();
//      }

// Check if the command starts with "out"
else if (command.substring(0, 3) == "out") {

  // Handle "get" command
  if (command.substring(7, 4) == "get") {
    String vdx = command.substring(11, 8); // Extract virtual device ID
    bool found = false;
    int i = 0;

    // Search for the virtual device in the list
    while (i < array_vd_en_lenght && !found) {
      if (vdx == cmd_vd[i]) {
        Serial.println(digitalRead(array_vd_en[i])); // Print digital value
        found = true;
      } else {
        i++;
      }
    }

    // Print result based on whether the device was found
    if (found) {
      print_ok(); // Success
    } else {
      print_cmd_nfound(); // Device not found
    }
  }

  // Handle "set" command
  else if (command.substring(7, 4) == "set") {

    // Check if the value is numeric
    if (is_numeric(command.substring(12))) {

      // Check if the value is within allowed range (0 or 1)
      if (command.substring(12).toInt() < 2) {
        String vdx = command.substring(11, 8); // Extract virtual device ID
        bool found = false;
        int i = 0;

        // Search for the virtual device in the list
        while (i < array_vd_en_lenght && !found) {
          if (vdx == cmd_vd[i]) {
            // Set the digital output
            DW_32u4(array_vd_en[i], command.substring(12).toInt());
            delay(200);

            // Special handling for "vco" device
            if (vdx == "vco" && (bool)command.substring(12).toInt()) {
              reg_calc();
              write_adf4108(INIT_Reg);
              write_adf4108(REF_Reg);
              write_adf4108(AB_Reg);
            }

            found = true;
          } else {
            i++;
          }
        }

        // Print result based on whether the device was found
        if (found) {
          print_ok(); // Success
        } else {
          print_cmd_nfound(); // Device not found
        }
      } else {
        print_exc_limit(); // Value out of allowed range
      }
    } else {
      print_isnt_num(); // Value is not numeric
    }
  }

  // If neither "get" nor "set" is matched
  else {
    print_cmd_nfound(); // Unknown sub-command
  }
}


//*************************set command*************************************
//      else if (command.substring(0, 3) == "set"){
//        byte space_pos = command.indexOf(' ');
////        Serial.print("space_pos = "); Serial.println(space_pos);
//        if (is_numeric(command.substring(space_pos + 1))){
//          double vout = command.substring(space_pos + 1).toDouble();
//          String setx = command.substring(space_pos, 0);
//          found = false;
//          int i = find_index_cmd_get_set (setx);
////          Serial.print("command = "); Serial.println(setx);
////          Serial.print("vout = "); Serial.println(vout, 3);
//          if (found){
////            Serial.print("comando trovato = "); Serial.print(setx); Serial.print(" indice = "); Serial.println(i);
//            if (vout >= array_set_reg_LL[i] && vout <= array_set_reg_LH[i]) {
//              double ofs = read_ofs(i);         
//              double k = read_k(i);            
//              double vda = 0;
//              byte calc = array_calc[i];
//              bool dis_isolator = array_dis_isolator[i];
//              if (calc == 1) {vda = (ofs - vout) / k;}
//              else if (calc == 2) {vda = (vout / k) + ofs;}
//              else if (calc == 4) {vda = (vout + ofs) * k;}
//              if (vda >= Vmin_AD5592 && vda <= Vmax_AD5592){
//                if (dis_isolator) {digitalWrite(EN_ISOLATOR, LOW);}
//                AW_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], vda); if (DAC_readback) {print_ok();} else print_error_DAC_readback();
//                if (dis_isolator) {digitalWrite(EN_ISOLATOR, HIGH);}
//              }
//              else print_vdac_exc();
//            }
//            else print_exc_limit();
//          }
//          else print_cmd_nfound();
//        }
//        else {print_isnt_num();}
//      }

// Check if the command starts with "set"
else if (command.substring(0, 3) == "set") {

  // Find the position of the first space character
  byte space_pos = command.indexOf(' ');

  // Check if the value after the space is numeric
  if (is_numeric(command.substring(space_pos + 1))) {

    // Convert the value to double
    double vout = command.substring(space_pos + 1).toDouble();

    // Extract the command keyword before the space
    String setx = command.substring(0, space_pos);

    found = false;

    // Find the index of the command in the list
    int i = find_index_cmd_get_set(setx);

    // If the command was found
    if (found) {

      // Check if the value is within the allowed range
      if (vout >= array_set_reg_LL[i] && vout <= array_set_reg_LH[i]) {

        // Read calibration parameters
        double ofs = read_ofs(i);
        double k = read_k(i);
        double vda = 0;

        // Get calculation method and isolator flag
        byte calc = array_calc[i];
        bool dis_isolator = array_dis_isolator[i];

        // Apply the appropriate formula based on the calculation method
        if (calc == 1) {
          vda = (ofs - vout) / k;
        } else if (calc == 2) {
          vda = (vout / k) + ofs;
        } else if (calc == 4) {
          vda = (vout + ofs) * k;
        }

        // Check if the calculated DAC value is within valid range
        if (vda >= Vmin_AD5592 && vda <= Vmax_AD5592) {

          // Disable isolator if required
          if (dis_isolator) {
            digitalWrite(EN_ISOLATOR, LOW);
          }

          // Write value to DAC
          AW_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], vda);

          // Confirm write success
          if (DAC_readback) {
            print_ok();
          } else {
            print_error_DAC_readback();
          }

          // Re-enable isolator if it was disabled
          if (dis_isolator) {
            digitalWrite(EN_ISOLATOR, HIGH);
          }

        } else {
          print_vdac_exc(); // DAC value out of range
        }

      } else {
        print_exc_limit(); // Output value out of allowed range
      }

    } else {
      print_cmd_nfound(); // Command not found
    }

  } else {
    print_isnt_num(); // Value is not numeric
  }
}


//*************************get command*************************************
//      else if (command.substring(0, 3) == "get"){
////        Serial.print("command = ");
////        Serial.println(command);
//        found = false;
//        int i = find_index_cmd_get_set (command);
////        Serial.print("found = ");
////        Serial.println(found);
////        Serial.print("i = ");
////        Serial.println(i);
//        if (found){   
//          double ofs = read_ofs(i);          
//          double k = read_k(i);
//          byte calc = array_calc[i];
//          bool dis_isolator = array_dis_isolator[i];
//          if (dis_isolator) {digitalWrite(EN_ISOLATOR, LOW);}
//          if (calc == 5) Serial.println((74.479 * (AR_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], ofs, k))) - 61.445, 1);//temp linear
//          else Serial.println(AR_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], ofs, k), 3);
//          if (dis_isolator) {digitalWrite(EN_ISOLATOR, HIGH);}
//          print_ok();
//        }
//        else print_cmd_nfound();
//      }

// Check if the command starts with "get"
else if (command.substring(0, 3) == "get") {

  // Optional debug output
  // Serial.print("command = ");
  // Serial.println(command);

  found = false;

  // Find the index of the command in the list
  int i = find_index_cmd_get_set(command);

  // Optional debug output
  // Serial.print("found = ");
  // Serial.println(found);
  // Serial.print("i = ");
  // Serial.println(i);

  // If the command was found
  if (found) {

    // Read calibration parameters
    double ofs = read_ofs(i);
    double k = read_k(i);
    byte calc = array_calc[i];
    bool dis_isolator = array_dis_isolator[i];

    // Disable isolator if required
    if (dis_isolator) {
      digitalWrite(EN_ISOLATOR, LOW);
    }

    // Special case: temperature linear conversion
    if (calc == 5) {
      Serial.println((74.479 * AR_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], ofs, k)) - 61.445, 1);
    } else {
      // Standard analog read with calibration
      Serial.println(AR_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i], ofs, k), 3);
    }

    // Re-enable isolator if it was disabled
    if (dis_isolator) {
      digitalWrite(EN_ISOLATOR, HIGH);
    }

    print_ok(); // Command executed successfully

  } else {
    print_cmd_nfound(); // Command not recognized
  }
}


//*************************reg command*************************************
    #ifdef COMMAND_READ_AD5592_REGISTER
      else if (command.substring(0, 3) == "reg"){
        found = false;
        int i = find_index_cmd_reg (command);
//        Serial.print("found = ");
//        Serial.println(found);
//        Serial.print("i = ");
//        Serial.println(i);
        if (found){
          double ofs = read_ofs(i);         
          double k = read_k(i);
          byte calc = array_calc[i];
          bool dis_isolator = array_dis_isolator[i];
          if (dis_isolator) {digitalWrite(EN_ISOLATOR, LOW);}
          double Readback_Reg = Readback_Reg_AD5592(array_set_reg_CE[i], array_set_reg_gpio[i]);
//          Serial.print("Readback_Reg = ");
//          Serial.println(Readback_Reg);
//          Serial.print("calc = ");
//          Serial.println(calc);
//          Serial.print("dis_isolator = ");
//          Serial.println(dis_isolator);
          if (dis_isolator) {digitalWrite(EN_ISOLATOR, LOW);}
          if (Readback_Reg <= Vmax_AD5592) {
            if (calc == 1){Serial.println(ofs - (Readback_Reg * k), 3);}
            else if (calc == 2){Serial.println((Readback_Reg - ofs) * k, 3);}
            else if (calc == 4){Serial.println((Readback_Reg / k) - ofs, 3);}
            print_ok();
          }               
          else print_error_DAC_readback();
          if (dis_isolator) {digitalWrite(EN_ISOLATOR, HIGH);}
        }
        else print_cmd_nfound();
      }
    #endif
      //      else if (command == "fvco"){
      //        Serial.println(fvco);
      //        print_ok();
      //      }

      // --- Handle "fvco" command ---
      else if (command == "fvco") {
          // Print the current value of fvco
          Serial.println(fvco);
          print_ok();
      }
      //      else if (command.substring(0, 4) == "fvco"){
      //        if ((command.substring(5).toDouble()) >= 1000 && (command.substring(17).toDouble()) <= 8000){
      //          fvco = command.substring(5).toDouble();
      //          EEPROM.put(10, fvco);
      //          reg_calc();
      //          write_adf4108(REF_Reg);
      //          write_adf4108(AB_Reg);
      //          write_adf4108(FUNCT_Reg);
      //          print_ok();
      //        }
      //        else {print_exc_limit();}
      //      }
      // --- Handle "fvco<value>" command for setting fvco ---
      else if (command.substring(0, 4) == "fvco") {
      
          // Extract the new fvco value from the command (starting after "fvco ")
          double new_fvco = command.substring(5).toDouble();
      
          // Validate that the new fvco value is within the allowed range
          if (new_fvco >= 1000 && new_fvco <= 8000) {
      
              // Update the fvco variable
              fvco = new_fvco;
      
              // Store the new value in EEPROM at address 10
              EEPROM.put(10, fvco);
      
              // Recalculate register values and write them to the ADF4108
              reg_calc();
              write_adf4108(REF_Reg);
              write_adf4108(AB_Reg);
              write_adf4108(FUNCT_Reg);
      
              // Indicate successful operation
              print_ok();
          } else {
              // Value is out of range, print error
              print_exc_limit();
          }
      }

 
      else {
        if (command != ""){
            print_cmd_nfound();
          }
        else{
        Serial.print(prompt);
          }        
      }
      command = "";
    }
    else{
      Serial.print((char)c);      
      command += (char)c;
    }
  }
}

void init_LL_LH() {
  // --- Load and validate EEPROM values ---
  EEPROM.get(100, VD_min);
  if (isnan(VD_min) || VD_min < 2 || VD_min > 8.4) {
    VD_min = 2;
    EEPROM.put(100, VD_min);
  }

  EEPROM.get(104, VD_max);
  if (isnan(VD_max) || VD_max < 2.1 || VD_max > 8.5) {
    VD_max = 8.5;
    EEPROM.put(104, VD_max);
  }

  EEPROM.get(108, VgGAAS_min);
  if (isnan(VgGAAS_min) || VgGAAS_min < -2.9 || VgGAAS_min > 2.8) {
    VgGAAS_min = -2.9;
    EEPROM.put(108, VgGAAS_min);
  }

  EEPROM.get(112, Vc_SIGE_max);
  if (isnan(Vc_SIGE_max) || Vc_SIGE_max < -2.8 || Vc_SIGE_max > 2.9) {
    Vc_SIGE_max = 2.9;
    EEPROM.put(112, Vc_SIGE_max);
  }

  EEPROM.get(116, Vif_min);
  if (isnan(Vif_min) || Vif_min < 0 || Vif_min > 9.4) {
    Vif_min = 0;
    EEPROM.put(116, Vif_min);
  }

  EEPROM.get(120, Vif_max);
  if (isnan(Vif_max) || Vif_max < 0.1 || Vif_max > 9.5) {
    Vif_max = 9.5;
    EEPROM.put(120, Vif_max);
  }

  EEPROM.get(124, Vref_cloop_min);
  if (isnan(Vref_cloop_min) || Vref_cloop_min < 0 || Vref_cloop_min > 2.3) {
    Vref_cloop_min = 0;
    EEPROM.put(124, Vref_cloop_min);
  }

  EEPROM.get(128, Vref_cloop_max);
  if (isnan(Vref_cloop_max) || Vref_cloop_max < 0.1 || Vref_cloop_max > 2.4) {
    Vref_cloop_max = 2.4;
    EEPROM.put(128, Vref_cloop_max);
  }

  EEPROM.get(132, Vg_pa_min);
  if (isnan(Vg_pa_min) || Vg_pa_min < -4 || Vg_pa_min > -2) {
    Vg_pa_min = -2;
    EEPROM.put(132, Vg_pa_min);
  }

  EEPROM.get(136, Vg_pa_max);
  if (isnan(Vg_pa_max) || Vg_pa_max < (Vg_pa_min + 0.1) || Vg_pa_max > 2.9) {
    Vg_pa_max = 2.9;
    EEPROM.put(136, Vg_pa_max);
  }

  // --- Initialize Low Limit (LL) array ---
  for (int i = 0; i < 5; i++) array_set_reg_LL[i] = VD_min;
  for (int i = 5; i < 14; i++) array_set_reg_LL[i] = 0;
  array_set_reg_LL[14] = VgGAAS_min;
  array_set_reg_LL[15] = Vmin_AD5592;
  for (int i = 16; i <= 23; i++) array_set_reg_LL[i] = VgGAAS_min;
  for (int i = 24; i <= 27; i++) array_set_reg_LL[i] = Vif_min;
  for (int i = 28; i <= 39; i++) array_set_reg_LL[i] = 0;
  for (int i = 40; i <= 43; i++) array_set_reg_LL[i] = Vref_cloop_min;
  for (int i = 44; i <= 47; i++) array_set_reg_LL[i] = Vg_pa_min;

  // --- Initialize High Limit (LH) array ---
  for (int i = 0; i < 5; i++) array_set_reg_LH[i] = VD_max;
  for (int i = 5; i < 14; i++) array_set_reg_LH[i] = 0;
  array_set_reg_LH[14] = Vc_SIGE_max;
  array_set_reg_LH[15] = Vmax_AD5592;
  for (int i = 16; i <= 23; i++) array_set_reg_LH[i] = Vc_SIGE_max;
  for (int i = 24; i <= 27; i++) array_set_reg_LH[i] = Vif_max;
  for (int i = 28; i <= 39; i++) array_set_reg_LH[i] = 0;
  for (int i = 40; i <= 43; i++) array_set_reg_LH[i] = Vref_cloop_max;
  for (int i = 44; i <= 47; i++) array_set_reg_LH[i] = Vg_pa_max;
}


int find_index_cmd_get_set (String command) {
//  Serial.print("command = ");
//  Serial.println(command);
  int index = 0;
  if (command == "set.vd1") {index = 0; found = true;}
  else if (command == "set.vd2") {index = 1; found = true;}
  else if (command == "set.vd3") {index = 2; found = true;}
  else if (command == "set.vd4") {index = 3; found = true;}
  else if (command == "set.vd5") {index = 4; found = true;}
  else if (command == "get.id3") {index = 5; found = true;}
  else if (command == "get.id4") {index = 6; found = true;}
  else if (command == "get.id5") {index = 7; found = true;}
  else if (command == "get.vd5") {index = 8; found = true;}
  else if (command == "get.vd4") {index = 9; found = true;}
  else if (command == "get.vd3") {index = 10; found = true;}
  else if (command == "get.vd2") {index = 11; found = true;}
  else if (command == "get.vd1") {index = 12; found = true;}
  else if (command == "get.temp") {index = 13; found = true;}
  else if (command == "set.ad2.6") {index = 14; found = true;}
//  else if (command == "set.ad2.7") {index = 15; found = true;}
  else if (command == "set.ad3.0") {index = 16; found = true;}
  else if (command == "set.ad3.1") {index = 17; found = true;}
  else if (command == "set.ad3.2") {index = 18; found = true;}
  else if (command == "set.ad3.3") {index = 19; found = true;}
  else if (command == "set.ad3.4") {index = 20; found = true;}
  else if (command == "set.ad3.5") {index = 21; found = true;}
  else if (command == "set.ad3.6") {index = 22; found = true;}
  else if (command == "set.ad3.7") {index = 23; found = true;}
  else if (command == "set.ad4.0") {index = 24; found = true;}
  else if (command == "set.ad4.1") {index = 25; found = true;}
  else if (command == "set.ad4.2") {index = 26; found = true;}
  else if (command == "set.ad4.3") {index = 27; found = true;}
  else if (command == "get.pll-lock") {index = 28; found = true;}
  else if (command == "get.ad4.5") {index = 29; found = true;}
  else if (command == "get.ad4.6") {index = 30; found = true;}
  else if (command == "get.ad4.7") {index = 31; found = true;}
  else if (command == "get.ad5.0") {index = 32; found = true;}
  else if (command == "get.ad5.1") {index = 33; found = true;}
  else if (command == "get.ad5.2") {index = 34; found = true;}
  else if (command == "get.ad5.3") {index = 35; found = true;}
  else if (command == "get.ad5.4") {index = 36; found = true;}
  else if (command == "get.ad5.5") {index = 37; found = true;}
  else if (command == "get.ad5.6") {index = 38; found = true;}
  else if (command == "get.ad5.7") {index = 39; found = true;}
  else if (command == "set.dtx.0") {index = 40; if (true){found = true;}}
  else if (command == "set.dtx.1") {index = 41; if (true){found = true;}}
  else if (command == "set.dtx.2") {index = 42; if (true){found = true;}}
  else if (command == "set.dtx.3") {index = 43; if (true){found = true;}}
  else if (command == "set.dtx.4") {index = 44; if (true){found = true;}}
  else if (command == "set.dtx.5") {index = 45; if (true){found = true;}}
  else if (command == "set.dtx.6") {index = 46; if (true){found = true;}}
  else if (command == "set.dtx.7") {index = 47; if (true){found = true;}}
//  Serial.print("found = ");
//  Serial.println(found);
//  Serial.print("index = ");
//  Serial.println(index);
  return index;
}

#ifdef COMMAND_READ_AD5592_REGISTER
  int find_index_cmd_reg (String command) {
  //  Serial.print("command = ");
  //  Serial.println(command);
    int index = 0;
    if (command == "reg.vd1") {index = 0; found = true;}
    else if (command == "reg.vd2") {index = 1; found = true;}
    else if (command == "reg.vd3") {index = 2; found = true;}
    else if (command == "reg.vd4") {index = 3; found = true;}
    else if (command == "reg.vd5") {index = 4; found = true;}
  //  else if (command == ".") {index = 5; found = true;}
  //  else if (command == ".") {index = 6; found = true;}
  //  else if (command == ".") {index = 7; found = true;}
  //  else if (command == ".") {index = 8; found = true;}
  //  else if (command == ".") {index = 9; found = true;}
  //  else if (command == ".") {index = 10; found = true;}
  //  else if (command == ".") {index = 11; found = true;}
  //  else if (command == ".") {index = 12; found = true;}
  //  else if (command == ".") {index = 13; found = true;}
    else if (command == "reg.ad2.6") {index = 14; found = true;}
    else if (command == "reg.ad2.7") {index = 15; found = true;}
    else if (command == "reg.ad3.0") {index = 16; found = true;}
    else if (command == "reg.ad3.1") {index = 17; found = true;}
    else if (command == "reg.ad3.2") {index = 18; found = true;}
    else if (command == "reg.ad3.3") {index = 19; found = true;}
    else if (command == "reg.ad3.4") {index = 20; found = true;}
    else if (command == "reg.ad3.5") {index = 21; found = true;}
    else if (command == "reg.ad3.6") {index = 22; found = true;}
    else if (command == "reg.ad3.7") {index = 23; found = true;}
    else if (command == "reg.ad4.0") {index = 24; found = true;}
    else if (command == "reg.ad4.1") {index = 25; found = true;}
    else if (command == "reg.ad4.2") {index = 26; found = true;}
    else if (command == "reg.ad4.3") {index = 27; found = true;}
  //  else if (command == ".") {index = 28; found = true;}
  //  else if (command == ".") {index = 29; found = true;}
  //  else if (command == ".") {index = 30; found = true;}
  //  else if (command == ".") {index = 31; found = true;}
  //  else if (command == ".") {index = 32; found = true;}
  //  else if (command == ".") {index = 33; found = true;}
  //  else if (command == ".") {index = 34; found = true;}
  //  else if (command == ".") {index = 35; found = true;}
  //  else if (command == ".") {index = 36; found = true;}
  //  else if (command == ".") {index = 37; found = true;}
  //  else if (command == ".") {index = 38; found = true;}
  //  else if (command == ".") {index = 39; found = true;}
    else if (command == "reg.dtx.0") {index = 40; if (true){found = true;}}
    else if (command == "reg.dtx.1") {index = 41; if (true){found = true;}}
    else if (command == "reg.dtx.2") {index = 42; if (true){found = true;}}
    else if (command == "reg.dtx.3") {index = 43; if (true){found = true;}}
    else if (command == "reg.dtx.4") {index = 44; if (true){found = true;}}
    else if (command == "reg.dtx.5") {index = 45; if (true){found = true;}}
    else if (command == "reg.dtx.6") {index = 46; if (true){found = true;}}
    else if (command == "reg.dtx.7") {index = 47; if (true){found = true;}}
  //  Serial.print("found = ");
  //  Serial.println(found);
  //  Serial.print("index = ");
  //  Serial.println(index);
    return index;
  }
#endif

//double read_ofs(int x){        
//  double ofs;
//  if (false){//(x > 39)
//    ofs = e2prom_get(add_str_ofs_M+(x*8));
//    if (isnan(ofs)) {ofs = 0; e2prom_put((add_str_ofs_M+(x*8)), ofs);}
//    else if (ofs < -1000 | ofs > 1000) {ofs = 0; e2prom_put((add_str_ofs_M+(x*8)), ofs);}
//  }
//  else{
//    EEPROM.get((add_str_ofs+(x*8)), ofs);
//    if (isnan(ofs)) {ofs = 0; EEPROM.put((add_str_ofs+(x*8)), ofs);}
//    else if (ofs < -1000 | ofs > 1000) {ofs = 0; EEPROM.put((add_str_ofs+(x*8)), ofs);}
//  }
////  Serial.print("ofs = ");
////  Serial.println(ofs);
//  return ofs;
//}

// Function to read a double offset value from EEPROM or custom memory
double read_ofs(int x) {
  double ofs;

  // This block is currently disabled (false condition)
  // It would read from a custom memory function (e2prom_get)
  if (false) { // (x > 39)
    ofs = e2prom_get(add_str_ofs_M + (x * 8));

    // If the value is NaN, reset to 0 and store it
    if (isnan(ofs)) {
      ofs = 0;
      e2prom_put(add_str_ofs_M + (x * 8), ofs);
    }
    // If the value is out of valid range, reset to 0 and store it
    else if (ofs < -1000 || ofs > 1000) {
      ofs = 0;
      e2prom_put(add_str_ofs_M + (x * 8), ofs);
    }
  }
  // Default block: read from standard EEPROM
  else {
    EEPROM.get(add_str_ofs + (x * 8), ofs);

    // If the value is NaN, reset to 0 and store it
    if (isnan(ofs)) {
      ofs = 0;
      EEPROM.put(add_str_ofs + (x * 8), ofs);
    }
    // If the value is out of valid range, reset to 0 and store it
    else if (ofs < -1000 || ofs > 1000) {
      ofs = 0;
      EEPROM.put(add_str_ofs + (x * 8), ofs);
    }
  }

  // Return the validated offset value
  return ofs;
}


//double read_k(int x){
//  double k;
//  if (false){//(x > 39)
//    k = e2prom_get(add_str_k_M+(x*8));
//    if (isnan(k)) {k = 1; e2prom_put((add_str_k_M+(x*8)), k);}
//    else if (k < -1000 | k > 1000) {k = 1; e2prom_put((add_str_k_M+(x*8)), k);}
//  }
//  else{
//    EEPROM.get((add_str_k+(x*8)), k);
//    if (isnan(k)) {k = 1; EEPROM.put((add_str_k+(x*8)), k);}
//    else if (k < -1000 | k > 1000) {k = 1; EEPROM.put((add_str_k+(x*8)), k);}
//  }
////  Serial.print("k = ");
////  Serial.println(k);
//  return k;
//}

// Function to read a double coefficient 'k' from EEPROM or custom memory
double read_k(int x) {
  double k;

  // This block is currently disabled (false condition)
  // It would read from a custom memory function (e2prom_get)
  if (false) { // (x > 39)
    k = e2prom_get(add_str_k_M + (x * 8));

    // If the value is NaN, reset to 1 and store it
    if (isnan(k)) {
      k = 1;
      e2prom_put(add_str_k_M + (x * 8), k);
    }
    // If the value is out of valid range, reset to 1 and store it
    else if (k < -1000 || k > 1000) {
      k = 1;
      e2prom_put(add_str_k_M + (x * 8), k);
    }
  }
  // Default block: read from standard EEPROM
  else {
    EEPROM.get(add_str_k + (x * 8), k);

    // If the value is NaN, reset to 1 and store it
    if (isnan(k)) {
      k = 1;
      EEPROM.put(add_str_k + (x * 8), k);
    }
    // If the value is out of valid range, reset to 1 and store it
    else if (k < -1000 || k > 1000) {
      k = 1;
      EEPROM.put(add_str_k + (x * 8), k);
    }
  }

  // Return the validated coefficient
  return k;
}


void print_cmdlist() {
  Serial.println();
  Serial.println(F("*******command list**********"));
//  Serial.println(F("error (error_code_list)"));
  Serial.println(F("F (read firmware version)"));
  Serial.println(F("S (read serial number)"));
  Serial.println(F("WE xxxx,xx = write byte eeprom ADD[0 to 1023],VALUE[0 to 255]"));
  Serial.println(F("RE xx = read byte eeprom ADD[0 to 1023]"));
  Serial.println(F("PE xxxx,xxxx = put double eeprom ADD[100 to 1023],VALUE[-1000 to 1000]"));
  Serial.println(F("GE xx = get double eeprom ADD[100 to 1023]"));
//  Serial.println(F("VD_min (volt)"));
//  Serial.println(F("VD_max (volt)"));
//  Serial.println(F("VgGAAS_min (volt)"));
//  Serial.println(F("Vc_SIGE_max (volt)"));
//  Serial.println(F("Vif_min (volt)"));
//  Serial.println(F("Vif_max (volt)"));
//  Serial.println(F("Vref_cloop_min (volt)"));
//  Serial.println(F("Vref_cloop_max (volt)"));
//  Serial.println(F("Vg_pa_min (volt)"));
//  Serial.println(F("Vg_pa_max (volt)"));
  Serial.println();

  Serial.println(F("PLL ADF4108:"));
  Serial.println(F("fvco [1000 to 8000 (MHz)]"));
//  Serial.println(F("ADF4108_REF [1 to 255 (MHz)]"));
//  Serial.println(F("ADF4108_wREF_Reg [after command send 3 bytes for Ref_Reg + 0xFF 0XFE 0xFD]"));
//  Serial.println(F("ADF4108_wAB_Reg [after command send 3 bytes for AB_Reg + 0xFF 0XFE 0xFD]"));
//  Serial.println(F("ADF4108_wFUNCT_Reg [after command send 3 bytes for FUNCT_Reg + 0xFF 0XFE 0xFD]"));
//  Serial.println(F("ADF4108_wINIT_Reg [after command send 3 bytes for INIT_Reg + 0xFF 0XFE 0xFD]"));
  Serial.println();
  
  Serial.println(F("32u4:"));
  Serial.println(F("out.set.vd[1-2-3-4-5-vco] [0->Disabled, 1->Enabled]"));
  Serial.println(F("out.get.vd[1-2-3-4-5-vco] (read state, 0->Disabled, 1->Enabled)"));
  Serial.println(F("idvco [read current(A)]\n"));
//  Serial.println();
  
  Serial.println(F("AD_1:"));
  Serial.print(F("set.vd[1-2-3-4-5-vco] [")); Serial.print(VD_min); Serial.print(F(" to ")); Serial.print(VD_max); Serial.println(F(" set volt]"));
  Serial.println(F("get.id3-4-5] (read current(A))"));
  Serial.println();

  Serial.println(F("AD_2:"));
  Serial.println(F("get.vd[1-2-3-4-5-temp] (read volt(V))"));
  Serial.print(F("set.ad2.6 [")); Serial.print(VgGAAS_min); Serial.print(F(" to ")); Serial.print(Vc_SIGE_max); Serial.println(F(" set volt]"));
  Serial.println();

  Serial.println(F("AD_3:"));
  Serial.print(F("set.ad3.[0 to 7] [")); Serial.print(VgGAAS_min); Serial.print(F(" to ")); Serial.print(Vc_SIGE_max); Serial.println(F(" set volt]"));
  Serial.println();
 
  Serial.println(F("AD_4:"));
  Serial.print(F("set.ad4.[0-1-2-3] [")); Serial.print(Vif_min); Serial.print(F(" to ")); Serial.print(Vif_max); Serial.println(F(" set volt]"));
  Serial.println(F("get.pll-lock [0-> UNLOCK, 3.3V->LOCKED]"));
  Serial.println(F("get.ad4.[5-6-7] (read volt(V))"));
  Serial.println();

  Serial.println(F("AD_5:"));
  Serial.println(F("get.ad5.[0 to 7] (read volt(V))"));
  Serial.println();

  Serial.println(F("AD_6:"));
//  Serial.println(F("cbl1.2 [read state]"));
//  Serial.println(F("cbl1.1 [read state]"));
//  Serial.println(F("cbl2.1 [read state]"));
//  Serial.println(F("cbl2.2 [read state]"));
  Serial.println(F("db.type [return type of DB]"));
//  Serial.println(F("mv.en [read state]"));
//  Serial.println(F("m3v.en [read state]"));
  Serial.println();

  Serial.println(F("AD5592_DTX:"));
  Serial.print(("set.dtx.[0-1-2-3]] [")); Serial.print(Vref_cloop_min); Serial.print(F(" to ")); Serial.print(Vref_cloop_max); Serial.println(F(" set volt]"));
//  Serial.print(("set.dtx.1 [")); Serial.print(Vref_cloop_min); Serial.print(F(" to ")); Serial.print(Vref_cloop_max); Serial.println(F(" set volt]"));
//  Serial.print(("set.dtx.2 [")); Serial.print(Vref_cloop_min); Serial.print(F(" to ")); Serial.print(Vref_cloop_max); Serial.println(F(" set volt]"));
//  Serial.print(("set.dtx.3 [")); Serial.print(Vref_cloop_min); Serial.print(F(" to ")); Serial.print(Vref_cloop_max); Serial.println(F(" set volt]"));
  Serial.print(("set.dtx.[4-5-6-7] [")); Serial.print(Vg_pa_min); Serial.print(F(" to ")); Serial.print(Vg_pa_max); Serial.println(F(" set volt]"));
//  Serial.print(("set.dtx.5 [")); Serial.print(Vg_pa_min); Serial.print(F(" to ")); Serial.print(Vg_pa_max); Serial.println(F(" set volt]"));
//  Serial.print(("set.dtx.6 [")); Serial.print(Vg_pa_min); Serial.print(F(" to ")); Serial.print(Vg_pa_max); Serial.println(F(" set volt]"));
//  Serial.print(("set.dtx.7 [")); Serial.print(Vg_pa_min); Serial.print(F(" to ")); Serial.print(Vg_pa_max); Serial.println(F(" set volt]"));

  Serial.println(F("*********************"));
}
//void print_error_code_list() {
//  for (int i=700; i<930; i++){
//    Serial.print((char)EEPROM.read(i));
//  }
//  Serial.println();
//  Serial.println(F("*******error list code**********"));
//  Serial.println(F("OK -> no error"));
//  Serial.println(F("1 -> error DAC readback register"));
//  Serial.println(F("2 -> value exceed the limits"));
//  Serial.println(F("3 -> v dac exceed the limits (ie calibration value strange value)"));
//  Serial.println(F("4 -> value is not a number"));
//  Serial.println(F("99 -> command not found"));
//
//  Serial.println(F("*********************"));
//}
void print_ok() {
  Serial.println(F("OK"));
  Serial.print(prompt); 
}
void print_error_DAC_readback() {
  Serial.println(F("DAC readback register error"));
  Serial.print(prompt); 
}
void print_exc_limit(){
  Serial.println(F("value exceed the limits"));
  Serial.print(prompt);
}
void print_vdac_exc(){
  Serial.println(F("v dac exceed the limits (ie calibration value strange value)"));
  Serial.print(prompt);
}
void print_isnt_num(){
  Serial.println(F("value is not a number"));
  Serial.print(prompt);
}
void print_cmd_nfound(){
  Serial.println(F("command not found"));
  Serial.print(prompt);
}

void conf_AD5592_1 (byte cs) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  init_AD5592 (cs);
  
// AD5592R ADC pin configuration (Reg Add 0100): IO5 to IO7 as ADC (analog input) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00100000); //D15-D8 (D15|0100|3xReserved)
  SPI.transfer(B11100000); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);

// AD5592R DAC pin configuration (Reg Add 0101): IO0 to IO4 as DAC (analog output) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00101000); //D15-D8 (D15|0101|3xReserved)
  SPI.transfer(B00011111); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);
}

void conf_AD5592_2 (byte cs) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  init_AD5592 (cs);
  
// AD5592R ADC pin configuration (Reg Add 0100): IO0 to IO5 as ADC (analog input) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00100000); //D15-D8 (D15|0100|3xReserved)
  SPI.transfer(B00111111); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);

// AD5592R DAC pin configuration (Reg Add 0101): IO6 as DAC (analog output) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00101000); //D15-D8 (D15|0101|3xReserved)
  SPI.transfer(B01000000); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);
}

void conf_AD5592_3 (byte cs) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1)); 
  init_AD5592 (cs);

// AD5592R DAC pin configuration (Reg Add 0101): IO0 to IO7 as DAC (analog output) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00101000); //D15-D8 (D15|0101|3xReserved)
  SPI.transfer(B11111111); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);
}

void conf_AD5592_4 (byte cs) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  init_AD5592 (cs);
  
// AD5592R ADC pin configuration (Reg Add 0100): IO4 to IO7 as ADC (analog input) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00100000); //D15-D8 (D15|0100|3xReserved)
  SPI.transfer(B11110000); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);

// AD5592R DAC pin configuration (Reg Add 0101): IO0 to IO3 as DAC (analog output) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00101000); //D15-D8 (D15|0101|3xReserved)
  SPI.transfer(B00001111); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);
}

void conf_AD5592_5 (byte cs) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  init_AD5592 (cs);
  
// AD5592R ADC pin configuration (Reg Add 0100): IO0 to IO7 as ADC (analog input) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00100000); //D15-D8 (D15|0100|3xReserved)
  SPI.transfer(B11111111); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);
}

// Configures the AD5592R device connected to chip select 'cs'
void conf_AD5592_6(byte cs) {
  // Begin SPI communication with specific settings:
  // - Clock speed: 500 kHz
  // - Bit order: MSB first
  // - SPI mode: 1
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));

  // Initialize the AD5592R device
  init_AD5592(cs);

  // --- Configure IO4 as ADC (analog input) ---
  // Register Address: 0x04 (ADC Enable)
  digitalWrite(cs, LOW);
  SPI.transfer(B00100000); // D15-D8: Command bits (Write to register 0x04)
  SPI.transfer(B00010000); // D7-D0: Enable ADC on IO4 (bit 4 set)
  digitalWrite(cs, HIGH);
  delay(5); // Short delay for stability

  // --- Configure GPIO Read Enable ---
  // Register Address: 0x0A (GPIO Read Configuration)
  // Enable GPIO input on IO0 to IO3, IO6, and IO7
  digitalWrite(cs, LOW);
  SPI.transfer(B01010000); // D15-D8: Command bits (Write to register 0x0A)
  SPI.transfer(B11001111); // D7-D0: Enable GPIO input on selected pins
  digitalWrite(cs, HIGH);
  delay(5); // Short delay for stability

  // Read back configuration or status (optional, depends on DR_AD5592 implementation)
  DR_AD5592(cs);

  // End SPI transaction (optional, depending on your SPI usage pattern)
  SPI.endTransaction();
}


void conf_AD5592_DTX (byte cs) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  init_AD5592 (cs);

// AD5592R DAC pin configuration (Reg Add 0101): IO0 to IO7 as DAC (analog output) 
  digitalWrite(cs, LOW);
  SPI.transfer(B00101000); //D15-D8 (D15|0101|3xReserved)
  SPI.transfer(B11111111); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(5);
}

void init_AD5592 (byte cs) {
// AD5592R SW reset (Reg Add 1111)
  digitalWrite(cs, LOW);
  SPI.transfer(B01111000); //D15-D8 (D15|1111|D10|D9|D8)
  SPI.transfer(B00000000); //D7-D0 (D7|D6|D5|D4|D3|D2|D1|D0)
  digitalWrite(cs, HIGH);
  delay(5);
  
// AD5592R set Ref_IN on (Reg Add 1011)
  digitalWrite(cs, LOW);
  SPI.transfer(B01011010); //D15-D8 (D15|1011|PA_ALL|EN_REF|Reserved)
  SPI.transfer(B00000000); //D7-D0 (PD7|PD6|PD5|PD4|PD3|PD2|PD1|PD0)
  digitalWrite(cs, HIGH);
  delay(5);
}

byte DR_AD5592 (byte cs){//AD5592 GPIO Read
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
// AD5592R GPIO Readback Register (Reg Add 1010)
  digitalWrite(cs, LOW);
  SPI.transfer(B01010100); //D15-D8 (D15|1010|GPIO_RD_EN|2xReserved)
  GPIO = SPI.transfer(0xff); //D7-D0 (IO7|IO6|IO5|IO4|IO3|IO2|IO1|IO0)
  digitalWrite(cs, HIGH);
  delay(1);
  
  return GPIO;
}

float AR_AD5592 (byte cs, byte gpio, double ofs, double k){//AD5592 ADC Read
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
// ADC Sequence Register (Reg Add 0010)
  digitalWrite(cs, LOW);
  SPI.transfer(B00010000); //D15-D8 (D15|0010|Reserved|Repeited|Temp)
  SPI.transfer(gpio); //D7-D0 (ADC7|ADC6|ADC5|ADC4|ADC3|ADC2|ADC1|ADC0)
  digitalWrite(cs, HIGH);
  delay(1);
  
  digitalWrite(cs, LOW);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  digitalWrite(cs, HIGH);
  delay(1);

  digitalWrite(cs, LOW);
  ADC0 = (uint16_t)SPI.transfer(0xff) << 8;
  ADC0 |= (uint16_t)SPI.transfer(0xff);
  digitalWrite(cs, HIGH);
  ADC0 = ADC0 & 0xFFF;
  
  digitalWrite(cs, LOW);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  digitalWrite(cs, HIGH);
  delay(1);
  
  float result = (((ADC0 / (float)4095) * Vref_AD5592) + (double)ofs) * (double)k;
  return result;
}

void AW_AD5592 (byte cs, byte gpio, float value){//AD5592 DAC Write then Readback Register to check
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));
  
  DAC0 = (uint16_t)((value/Vref_AD5592) * (int)4095);
//  Serial.print("DAC0 = ");
//  Serial.println(DAC0);
//DAC Write
  D15D8 = 0x80 | (gpio << 4) | (DAC0 >> 8);
  D7D0 = DAC0 & 0xFF;
  
  digitalWrite(cs, LOW);
  SPI.transfer(D15D8); //D15-D8 (D15|DAC_ADDR2|DAC_ADDR1|DAC_ADDR0|DAC11|DAC10|DAC9|DAC8)
  SPI.transfer(D7D0); //D7-D0 (DAC7|DAC6|DAC5|DAC4|DAC3|DAC2|DAC1|DAC0)
  digitalWrite(cs, HIGH);
  delay(2);

//DAC Readback Register (Reg Add 0001)
  D15D8 = B00001000;
  D7D0 = B00011000 | gpio;
//  Serial.print("D7D0 = 0b");
//  Serial.println(D7D0, BIN);
  digitalWrite(cs, LOW);
  SPI.transfer(D15D8); //D15-D8 (D15|0001|3 x Reserved)
  SPI.transfer(D7D0); //D7-D0 (3 x Reserved|DAC_RD_EN1|DAC_RD_EN0|DAC_CH_SEL2|DAC_CH_SEL1|DAC_CH_SEL0)
  digitalWrite(cs, HIGH);
  delay(1);

  digitalWrite(cs, LOW);
  ADC0 = (uint16_t)SPI.transfer(0x00) << 8;
  ADC0 |= (uint16_t)SPI.transfer(0x00);
  digitalWrite(cs, HIGH);
  
  ADC0 = ADC0 & 0xFFF;
//  Serial.print("ADC0 = ");
//  Serial.println(ADC0);
  DAC_readback = false;
  if (ADC0 == DAC0) {DAC_readback = true;}
}

float Readback_Reg_AD5592 (byte cs, byte gpio){//AD5592 DAC Write raw
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE1));

//DAC Readback Register (Reg Add 0001)
  D15D8 = B00001000;
  D7D0 = B00011000 | gpio;
  digitalWrite(cs, LOW);
  SPI.transfer(D15D8); //D15-D8 (D15|0001|3 x Reserved)
  SPI.transfer(D7D0); //D7-D0 (3 x Reserved|DAC_RD_EN1|DAC_RD_EN0|DAC_CH_SEL2|DAC_CH_SEL1|DAC_CH_SEL0)
  digitalWrite(cs, HIGH);
  delay(1);

  digitalWrite(cs, LOW);
  ADC0 = (uint16_t)SPI.transfer(0x00) << 8;
  ADC0 |= (uint16_t)SPI.transfer(0x00);
  digitalWrite(cs, HIGH);
  ADC0 = ADC0 & 0xFFF;
//  Serial.print("ADC0 = ");
//  Serial.println(ADC0);
  float result = ((ADC0 / (float)4095) * Vref_AD5592);
  return result;
}

void write_adf4108(uint32_t reg) {
  SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
  digitalWrite(LE_ADF4108, LOW);
  SPI.transfer((reg >> 16)) & 0xFF; //D23-16 (D23|D22|D21|D20|D19|D18|D17|D16)
  SPI.transfer((reg >> 8) & 0xFF); //D15-D8 (D15|D14|D13|D12|D11|D10|D9|D8)
  SPI.transfer(reg & 0xFF); //D7-D0 (D7|D6|D5|D4|D3|D2|D1|D0)
  digitalWrite(LE_ADF4108, HIGH);
  delay(1);
}
/*
//void ADF4108_wreg (uint32_t sReg) {
  wReg = sReg;
  for (int i = 2; i > -1; i--) {
    wReg |= (uint32_t)WaitAndReadOneByte() << (i*8);
  }
  // 3 bytes end of message, it must be (0xFF)(0xFE)(0xFD) to be taken into account, it's a super simple way to be sure we reached the correct end.
  valid = true;
  bool check;
  for (int i = 0; i < 3; i++) {
    check = (WaitAndReadOneByte() == 0xFF - i);
    valid = valid && check;
  }
//  Serial.print("wReg = 0x");
//  Serial.println(wReg, HEX);
//  Serial.print("valid = 0x");
//  Serial.println(valid);
}
*/
void DW_32u4 (byte gpio, byte value){
  digitalWrite (gpio, value);
}

float AR_32u4 (int ai, double ofs, double k){
//  Serial.print("A0 = ");
//  Serial.println(analogRead(A0));
//  Serial.print("A1 = ");
//  Serial.println(analogRead(A1));
//  Serial.print("A4 = ");
//  Serial.println(analogRead(A4));
  float result  = (((analogRead(ai) / (float)1023) * Vref_32u4) + ofs) * k;
  return result;
}

//bool is_numeric(String param) {
//  int i;
//  bool d = true;
//  if (param.length() > 0 && param[0] != '.'){
//      for (i=0; i<param.length(); i++) {
//      if (!(isDigit(param[i]) | (param[i] == '.' | (param[i] == '-')))) d = false;
//    }
//  }
//  else d = false;
////  Serial.print("is_numeric = ");
////  Serial.println(d);  
//  return d;
//}


// Function to check if a string represents a numeric value
bool is_numeric(const String& param) {
  // Return false if the string is empty or starts with a dot
  if (param.length() == 0 || param[0] == '.') return false;

  // Loop through each character in the string
  for (int i = 0; i < param.length(); i++) {
    char c = param[i];

    // If the character is not a digit, a dot, or a minus sign, return false
    if (!isDigit(c) && c != '.' && c != '-') {
      return false;
    }
  }

  // If all characters are valid, return true
  return true;
}



//int WaitAndReadOneByte() {
//  while (!Serial.available()) {}
//  return Serial.read();
//}

void reg_calc() {
  REF_Reg = 0b00;
  AB_Reg = 0b01;
  FUNCT_Reg = 0b10;
  INIT_Reg = 0b11;

  ADF4108_PFD = (double)ADF4108_REF / (double)R;
//  Serial.print("ADF4108_PFD = ");
//  Serial.println(ADF4108_PFD, 10);
  
  N = (uint16_t)(fvco / ADF4108_PFD);
//  Serial.print("N = ");
//  Serial.println(N);

  B = N / PRESCALER;
//  Serial.print("B = ");
//  Serial.println(B);

  A = N % PRESCALER;
//  Serial.print("A = ");
//  Serial.println(A, BIN);

  AB_Reg |= ((A & 0b111111) << 2);
  AB_Reg |= ((uint32_t)(B & 0x1FFF) << 8);
  AB_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_GAIN & 1) << 21);
//  Serial.print("AB_Reg = ");
//  Serial.println(AB_Reg, HEX);

  REF_Reg |= ((uint32_t)(R & 0x3FFF) << 2);
  REF_Reg |= ((uint32_t)(ADF4108_ABPW & 0x3) << 16);
  REF_Reg |= ((uint32_t)(ADF4108_LOCK_DET_PRECISION & 1) << 20);
//  Serial.print("REF_Reg = ");
//  Serial.println(REF_Reg, HEX);

  FUNCT_Reg |= ((uint32_t)(ADF4108_COUNTER_RES & 1) << 2);
  FUNCT_Reg |= ((uint32_t)(ADF4108_MUX_OUT & 7) << 4);
  FUNCT_Reg |= ((uint32_t)(ADF4108_PHASE_DET_POL & 1) << 7);
  FUNCT_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_3ST & 1) << 8);
  FUNCT_Reg |= ((uint32_t)(ADF4108_FAST_LOCK & 3) << 9);
  FUNCT_Reg |= ((uint32_t)(ADF4108_TIMEOUT & 15) << 11);
  FUNCT_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_SET1 & 7) << 15);
  FUNCT_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_SET2 & 7) << 18);
  FUNCT_Reg |= ((uint32_t)(ADF4108_PWR_DOWN & 1) << 3);
  FUNCT_Reg |= ((uint32_t)((ADF4108_PWR_DOWN & 2) >> 1) << 21);
  FUNCT_Reg |= ((uint32_t)(ADF4108_PRES & 3) << 22);
//  Serial.print("FUNCT_Reg = ");
//  Serial.println(FUNCT_Reg, HEX);

  INIT_Reg |= ((uint32_t)(ADF4108_COUNTER_RES & 1) << 2);
  INIT_Reg |= ((uint32_t)(ADF4108_MUX_OUT & 7) << 4);
  INIT_Reg |= ((uint32_t)(ADF4108_PHASE_DET_POL & 1) << 7);
  INIT_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_3ST & 1) << 8);
  INIT_Reg |= ((uint32_t)(ADF4108_FAST_LOCK & 3) << 9);
  INIT_Reg |= ((uint32_t)(ADF4108_TIMEOUT & 15) << 11);
  INIT_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_SET1 & 7) << 15);
  INIT_Reg |= ((uint32_t)(ADF4108_CRG_PUMP_SET2 & 7) << 18);
  INIT_Reg |= ((uint32_t)(ADF4108_PWR_DOWN & 1) << 3);
  INIT_Reg |= ((uint32_t)((ADF4108_PWR_DOWN & 2) >> 1) << 21);
  INIT_Reg |= ((uint32_t)(ADF4108_PRES & 3) << 22);
//  Serial.print("INIT_Reg = ");
//  Serial.println(INIT_Reg, HEX);
}

double e2prom_get (int address){
  byte data[4];
  double result = 10;
  digitalWrite(e2prom_CE, LOW);
  SPI.transfer(EEPROM_READ_MEMORY_ARRAY); //transmit read opcode
  SPI.transfer((char)(address>>8));  //send MSByte address first
  SPI.transfer((char)(address)); //send LSByte address
  for (int I=0; I<4; I++){
    data[I] = SPI.transfer(0xFF);
//    Serial.print("data ["); Serial.print(I); Serial.print("] = "); Serial.println(data[I]);
  }
  digitalWrite(e2prom_CE, HIGH);
  memcpy(&result, &data, 4);
  return result;
}

byte e2prom_read (int address){
  byte result;
  digitalWrite(e2prom_CE, LOW);
  SPI.transfer(EEPROM_READ_MEMORY_ARRAY); //transmit read opcode
  SPI.transfer((char)(address>>8));  //send MSByte address first
  SPI.transfer((char)(address)); //send LSByte address
  result = SPI.transfer(0xFF);
  digitalWrite(e2prom_CE, HIGH);
  return result;
}

void e2prom_write (int address, byte data){
  start_add_page = ((int)address/page_size) * page_size;
  byte buffer_read[page_size];

  digitalWrite(e2prom_CE, LOW);
  SPI.transfer(EEPROM_READ_MEMORY_ARRAY); //transmit read opcode
  SPI.transfer((char)(start_add_page>>8));  //send MSByte address first
  SPI.transfer((char)(start_add_page)); //send LSByte address
  for (int I=0; I<page_size; I++){
    buffer_read[I] = SPI.transfer(0xFF);
  }
  digitalWrite(e2prom_CE, HIGH);
//  delay(10);
  
  buffer_read[address - start_add_page] = data;
  e2prom_write_page (start_add_page, buffer_read);
}

void e2prom_put (int address, double data){
  start_add_page = ((int)address/page_size) * page_size;
  byte buffer_read[page_size];
  
  for (int I=0; I<page_size; I++){
    digitalWrite(e2prom_CE, LOW);
    SPI.transfer(EEPROM_READ_MEMORY_ARRAY); //transmit read opcode
    SPI.transfer((char)((start_add_page+I)>>8));  //send MSByte address first
    SPI.transfer((char)(start_add_page+I)); //send LSByte address
    buffer_read[I] = SPI.transfer(0xFF);
    digitalWrite(e2prom_CE, HIGH);
    delay(5);
  }
  doub2arr.num = data;
//  Serial.print("doub2arr.num = "); Serial.println(doub2arr.num, 3);
  buffer_read[address - start_add_page] = data;
  for (int i=0; i<4; i++){
    buffer_read[address - start_add_page + i] = doub2arr.array[i];
//    Serial.print("doub2arr.array = "); Serial.println(doub2arr.array[i]);
  }
  e2prom_write_page (start_add_page, buffer_read);
}

//void e2prom_write_page (int address, byte data_page[page_size]){
//  digitalWrite(e2prom_CE ,LOW);
//  SPI.transfer(EEPROM_WRITE_ENABLE); //write enable
//  digitalWrite(e2prom_CE, HIGH);
//  delay(5);
//  byte buffer_write[page_size];
//  for (int i=0; i<page_size; i++){
//    buffer_write[i] = data_page[i];
//  }
//  digitalWrite(e2prom_CE, LOW);
//  SPI.transfer(EEPROM_WRITE_MEMORY_ARRAY); //write instruction
//  SPI.transfer((char)(address>>8));  //send MSByte address first
//  SPI.transfer((char)(address)); //send LSByte address
//  for (int I=0; I<page_size; I++){
//    SPI.transfer(buffer_write[I]);//write data byte
//    delay(5);
//  }
//  digitalWrite(e2prom_CE, HIGH);
//  //wait for eeprom to finish writing
//  delay(100);
//}

// Scrive una pagina di dati nella EEPROM via SPI
void e2prom_write_page(int address, const byte data_page[page_size]) {
  // Abilita la scrittura sulla EEPROM
  digitalWrite(e2prom_CE, LOW);
  SPI.transfer(EEPROM_WRITE_ENABLE);
  digitalWrite(e2prom_CE, HIGH);
  delay(5); // attesa minima per stabilità
  // Copia i dati in un buffer locale (opzionale se data_page è già sicuro)
  byte buffer_write[page_size];
  memcpy(buffer_write, data_page, page_size);

  // Inizio della scrittura nella memoria
  digitalWrite(e2prom_CE, LOW);
  SPI.transfer(EEPROM_WRITE_MEMORY_ARRAY); // comando di scrittura

  // Invio dell'indirizzo (MSB prima, poi LSB)
  SPI.transfer((uint8_t)(address >> 8));   // byte più significativo
  SPI.transfer((uint8_t)(address & 0xFF)); // byte meno significativo

  // Scrittura dei dati byte per byte
  for (int i = 0; i < page_size; i++) {
    SPI.transfer(buffer_write[i]);
  }

  digitalWrite(e2prom_CE, HIGH);

  // Attesa per completamento scrittura (valore da datasheet)
  delay(100);
}

//bool e2prom_is_connect(){
//  digitalWrite(e2prom_CE ,LOW);
//  SPI.transfer(EEPROM_WRITE_ENABLE); //write enable
//  digitalWrite(e2prom_CE, HIGH);
//  delay(5);
//
//  delayMicroseconds(10);
//
//  digitalWrite(e2prom_CE ,LOW);
//  SPI.transfer(EEPROM_READ_STATUS_REGISTER);
//  int read_status_reg = SPI.transfer(0xFF);
//  digitalWrite(e2prom_CE, HIGH);
//
//  if(read_status_reg != 0xFF && read_status_reg & (0x01 << 1)){
//    // WEL bit is set, so we are talking with the EEPROM!
//    // Let's go and disable it again
//    digitalWrite(e2prom_CE ,LOW);
//    SPI.transfer(EEPROM_WRITE_DISABLE);
//    digitalWrite(e2prom_CE, HIGH);
//    return true;
//  }
//  return false;
//}
