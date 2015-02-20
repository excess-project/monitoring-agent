#!/bin/bash
TEMPLATE_FILE="mf_config.template"
USER=$USER
HOME_DIR=$HOME

## Prepare
rm -rf configurations
mkdir -p configurations

## Configuration
declare -a PLUGINS=("mf_plugin_papi" "mf_plugin_rapl" "mf_plugin_likwid" "mf_plugin_meminfo")
declare -a UPDATE_FREQUENCIES=(10 25 50 100 200 300 400 500 1000)


## Generate configuration files [SINGLE]
for UPDATE_FREQUENCY in "${UPDATE_FREQUENCIES[@]}"; do
    for PLUGIN_A in "${PLUGINS[@]}"; do
      DESTINATION="configurations/mf_config-${PLUGIN_A}-${UPDATE_FREQUENCY}ms.ini"
      cp $TEMPLATE_FILE $DESTINATION

      ## Write [plugins]
      echo -e "\n\n[plugins]" >> $DESTINATION
      if [ -n "$PLUGIN_A" ]; then
        echo -e "$PLUGIN_A = on" >> $DESTINATION
      fi
      for PLUGIN_B in "${PLUGINS[@]}"; do
        if [ "$PLUGIN_A" == "$PLUGIN_B" ]; then
          continue
        fi
        echo -e "$PLUGIN_B = off" >> $DESTINATION
      done

      ## Write [timings]
      echo -e "\n\n[timings]\ndefault = 1000000000\npublish_data_interval = 0\nupdate_configuration = 360" >> $DESTINATION
      for PLUGIN in "${PLUGINS[@]}"; do
        NANOSECONDS=$((${UPDATE_FREQUENCY} * 1000000))
        if [ -n "$PLUGIN" ]; then
          echo "$PLUGIN = $NANOSECONDS" >> $DESTINATION
        fi
      done
    done
done

## Generate configuration files [PAPI+OTHER]
ITERATIONS=$((${#PLUGINS[@]} - 1))
for UPDATE_FREQUENCY in "${UPDATE_FREQUENCIES[@]}"; do
    INDEX=1
    for ((PLUGIN_A=0; PLUGIN_A<${ITERATIONS}; PLUGIN_A++)); do
      DESTINATION="configurations/mf_config-${PLUGINS[0]}-${PLUGINS[INDEX]}-${UPDATE_FREQUENCY}ms.ini"
      cp $TEMPLATE_FILE $DESTINATION

      ## Write [plugins]
      echo -e "\n\n[plugins]\n${PLUGINS[0]} = on" >> $DESTINATION
      for i in "${!PLUGINS[@]}"; do
        if [ "${PLUGINS[0]}" == "${PLUGINS[$i]}" ]; then
          continue
        fi
        if [ $INDEX -eq $i ]; then
          echo -e "${PLUGINS[$i]} = on" >> $DESTINATION
        else
          echo -e "${PLUGINS[$i]} = off" >> $DESTINATION
        fi
      done

      ## Write [timings]
      echo -e "\n\n[timings]\ndefault = 1000000000\npublish_data_interval = 0\nupdate_configuration = 360" >> $DESTINATION
      for PLUGIN in "${PLUGINS[@]}"; do
        NANOSECONDS=$((${UPDATE_FREQUENCY} * 1000000))
        echo "$PLUGIN = $NANOSECONDS" >> $DESTINATION
      done

      INDEX=$[$INDEX +1]
    done
done
