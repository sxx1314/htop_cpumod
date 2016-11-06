/*
htop - CPUFreqMeter.c
(C) 2016 Petr Benes
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/


#include "CPUFreqMeter.h"

#include "CRT.h"
#include "ProcessList.h"
#include "StringUtils.h"

#define SYS_CPU_DIR "/sys/devices/system/cpu"

/*{
#include "Meter.h"
}*/

int CPUFreqMeter_attributes[] = {
   CPUFREQ_LOW,
   CPUFREQ_MEDIUM,
   CPUFREQ_HIGH,
};

static void CPUFreqMeter_updateValues(Meter* this, char* buffer, int len) {
   this->values[0] = 0.0;

   // get current frequency
   {
      FILE* file;
      file = fopen(SYS_CPU_DIR "/cpu0/cpufreq/scaling_cur_freq", "r");
      if (!file)
        return;

      char *line = NULL;
      line = String_readLine(file);
      fclose(file);
      if (!line)
        return;

      int cpufreq;
      cpufreq = strtol(line, NULL, 10);
      free(line);

      cpufreq /= 1000;
      this->values[0] = (double)cpufreq;
   }

   // get max frequency
   {
      FILE* file;
      file = fopen(SYS_CPU_DIR "/cpu0/cpufreq/scaling_max_freq", "r");
      if (!file)
        return;

      char *line = NULL;
      line = String_readLine(file);
      fclose(file);
      if (!line)
        return;

      int cpufreq;
      cpufreq = strtol(line, NULL, 10);
      free(line);

      cpufreq /= 1000;
      this->total = (double)cpufreq;
   }

   snprintf(buffer, len, "%d/%d", (int)this->values[0], (int)this->total);
}

static void CPUFreqMeter_display(Object* cast, RichString* out) {
   Meter* this = (Meter*)cast;

   // choose the color for the frequency
   int cpufreq = (int)this->values[0];
   int freqColor;
   if (cpufreq < 1000)                         freqColor = CRT_colors[CPUFREQ_LOW];
   else if (cpufreq >= 1000 && cpufreq < 2000) freqColor = CRT_colors[CPUFREQ_MEDIUM];
   else                                        freqColor = CRT_colors[CPUFREQ_HIGH];

   // output the frequence
   char buffer[20];
   sprintf(buffer, "%d", cpufreq);
   RichString_append(out, freqColor, buffer);
   RichString_append(out, CRT_colors[METER_TEXT], " MHz");
}

MeterClass CPUFreqMeter_class = {
   .super = {
      .extends = Class(Meter),
      .display = CPUFreqMeter_display,
      .delete = Meter_delete,
   },
   .updateValues = CPUFreqMeter_updateValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = 1,
   .total = 0.0,
   .attributes = CPUFreqMeter_attributes,
   .name = "CPUFreq",
   .uiName = "CPU Frequency",
   .caption = "CPU Freq: "
};
