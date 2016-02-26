
/* This is a source code for the arduino (MEGA 2560)
The arduino is connected to the movidius MV0198 Daughtercard over the I2C interface. The Daughtercard has four MCP3424 chips, which measure the power comsumption of various movidius hardware components.
The measured values are pushed to the serial interface and can be read by the host system (e.x. with mf_movidius_plugin).
The external library MCP3424 is used.

Bugs report to: khabi@hlrs.de
High Performance Computing Center Stuttgart (HLRS)
University of Stuttgart
*/
#include <MCP3424.h>
#include <Wire.h>


MCP3424MY MCP3424_A(110); // channel 1-VDDCR CURRENT 20 mV per ma; 2-VDDCV CURRENT 0.6 mV per ma; 3-DRAM_VDD1 CURRENT 20 mV per ma; 4-MIPI_VDD CURRENT 20 mV per ma;
MCP3424MY MCP3424_B(109); // channel 1-DRAM_VDD2 CURRENT 20 mV per ma; 2-DRAM_VDDQ CURRENT 10 mV per ma; 3-DRAM_MVDDQ CURRENT 2 mV per ma; 4-DRAM_MVDDA CURRENT 20 mV per ma;
MCP3424MY MCP3424_C(106); // channel 1-USB_VDD330 CURRENT 20 mV per ma; 2-USB_VP_VDD CURRENT 20 mV per ma; 3-VDDIO CURRENT 10 mV per ma; 4-VDDIO_B CURRENT 20 mV per ma;
MCP3424MY MCP3424_D(107); // channel 1-RESERVED RESERVED X mV per ma; 2-PLL_AVDD CURRENT 20 mV per ma; 3-VDDCV VOLTAGE 1 per 1; 4-MIPI_VDD VOLTAGE 1 per 1;
long count = 0;
long Voltage[4][4]; // Array used to store results
void setup() {
  String config_str;
  Serial.begin(115200);  // start serial for output
}

void loop(){
  while(true)
  {
   count = count+1;
    for(int i=1;i<=4;i++){
      MCP3424_A.Configuration(i,12,1,1);  
      MCP3424_B.Configuration(i,12,1,1);  
      MCP3424_C.Configuration(i,12,1,1);  
      MCP3424_D.Configuration(i,12,1,1);  
      Voltage[0][i-1]=MCP3424_A.Measure(); // read the chip measurement
      Voltage[1][i-1]=MCP3424_B.Measure(); // read the chip measurement    
      Voltage[2][i-1]=MCP3424_C.Measure(); // read the chip measurement
      Voltage[3][i-1]=MCP3424_D.Measure(); // read the chip measurement
    }
    String stringData; 
    stringData=stringData+"#"+count+";";
    for(int j=0;j<=3;j++)
    {

      for(int i=0;i<=3;i++)
      {
        stringData=stringData+Voltage[j][i]+";";
      }
    }
   
     Serial.print(stringData); // print results

  
    //delay(100);
  }
}

