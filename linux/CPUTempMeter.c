/*
htop - CPUTempMeter.c
(C) 2016 Petr Benes
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "CPUTempMeter.h"

#include "CRT.h"
#include "ProcessList.h"
#include "StringUtils.h"

#include <limits.h>

#define SYS_THERMAL_DIR "/sys/class/thermal"

/*{
#include "Meter.h"
}*/

int CPUTempMeter_attributes[] = {
   TEMPERATURE_COOL,
   TEMPERATURE_MEDIUM,
   TEMPERATURE_HOT,
};

static void CPUTempMeter_updateValues(Meter* this, char* buffer, int len) {
   this->values[0] = 0.0;

   FILE* file;
   file = fopen(SYS_THERMAL_DIR "/thermal_zone0/temp", "r");
   if (!file)
     return;

   char* line = NULL;
   line = String_readLine(file);
   fclose(file);
   if (!line)
     return;

   double temperature;
   temperature = strtod(line, NULL);
   free(line);
   // some of the systems(e.g Orange Pi) show real temperature in Celcius and therefore we do not need to divide by 1000 
   if (temperature > 1000.0) temperature /= 1000.0;
 
   this->values[0] = temperature;

   snprintf(buffer, len, "%d/%d", (int)this->values[0], (int)this->total);
}

static void CPUTempMeter_display(Object* cast, RichString* out) {
   Meter* this = (Meter*)cast;

   // choose the color for the temperature
   double temperature = this->values[0];
   int tempColor;
   if      (temperature < 60.0)                        tempColor = CRT_colors[TEMPERATURE_COOL];
   else if (temperature >= 60.0 && temperature < 80.0) tempColor = CRT_colors[TEMPERATURE_MEDIUM];
   else                                                tempColor = CRT_colors[TEMPERATURE_HOT];

   // output the temperature
   char buffer[20];
   sprintf(buffer, "%.1f", temperature);
   RichString_append(out, tempColor, buffer);
   RichString_append(out, CRT_colors[METER_TEXT], "\xc2\xb0" "C");
}

MeterClass CPUTempMeter_class = {
   .super = {
      .extends = Class(Meter),
      .display = CPUTempMeter_display,
      .delete = Meter_delete,
   },
   .updateValues = CPUTempMeter_updateValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = 1,
   .total = 100.0,
   .attributes = CPUTempMeter_attributes,
   .name = "CPUTemp",
   .uiName = "CPU Temp",
   .caption = "CPU Temp: "
};
