#!/system/bin/sh
# Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

echo 4 > /sys/module/lpm_levels/enable_low_power/l2
echo 1 > /sys/module/msm_pm/modes/cpu0/power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu1/power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu2/power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu3/power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu0/power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu1/power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu2/power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu3/power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu0/standalone_power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu1/standalone_power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu2/standalone_power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu3/standalone_power_collapse/suspend_enabled
echo 1 > /sys/module/msm_pm/modes/cpu0/standalone_power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu1/standalone_power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu2/standalone_power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu3/standalone_power_collapse/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu0/retention/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu1/retention/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu2/retention/idle_enabled
echo 1 > /sys/module/msm_pm/modes/cpu3/retention/idle_enabled
echo 0 > /sys/module/msm_thermal/core_control/enabled
echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online
for devfreq_gov in /sys/class/devfreq/qcom,cpubw*/governor
do
    echo "cpubw_hwmon" > $devfreq_gov
done
echo "interactive" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "interactive" > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
echo "interactive" > /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor
echo "interactive" > /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor

echo 10 > /sys/module/cpu_boost/parameters/boost_ms
echo 1497600 > /sys/module/cpu_boost/parameters/sync_threshold
echo 0 > /sys/module/cpu_boost/parameters/input_boost_freq
echo 40 > /sys/module/cpu_boost/parameters/input_boost_ms
echo 1 > /dev/cpuctl/apps/cpu.notify_on_migrate
echo 300000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
echo 300000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq
echo 300000 > /sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq
echo 300000 > /sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq
echo 1 > /sys/module/msm_thermal/core_control/enabled
setprop ro.qualcomm.perf.cores_online 2
chown -h system /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
chown -h system /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq
chown -h root.system /sys/devices/system/cpu/mfreq
chmod -h 220 /sys/devices/system/cpu/mfreq
chown -h root.system /sys/devices/system/cpu/cpu1/online
chown -h root.system /sys/devices/system/cpu/cpu2/online
chown -h root.system /sys/devices/system/cpu/cpu3/online
chmod -h 664 /sys/devices/system/cpu/cpu1/online
chmod -h 664 /sys/devices/system/cpu/cpu2/online
chmod -h 664 /sys/devices/system/cpu/cpu3/online

# Change interactive sysfs permission
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/timer_rate
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/timer_slack
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/min_sample_time
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/target_loads
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/boost
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/boostpulse
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/input_boost
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/boostpulse_duration
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/io_is_busy
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/enforced_mode
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/mode
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/multi_enter_load
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/multi_enter_time
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/multi_exit_load
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/multi_exit_time
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/param_index
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/single_enter_load
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/single_enter_time
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/single_exit_load
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/single_exit_time
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/sync_freq
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/up_threshold_any_cpu_freq
chown -h system.system /sys/devices/system/cpu/cpufreq/interactive/up_threshold_any_cpu_load
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/timer_rate
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/timer_slack
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/min_sample_time
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/target_loads
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/boost
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/input_boost
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/boostpulse_duration
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/io_is_busy
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor

# Mode Change Condition
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/param_index
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/multi_enter_load
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/multi_enter_time
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/multi_exit_load
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/multi_exit_time
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/single_enter_load
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/single_enter_time
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/single_exit_load
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/single_exit_time
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/enforce_mode
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/mode
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/sync_freq
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/up_threshold_any_cpu_freq
chmod -h 0660 /sys/devices/system/cpu/cpufreq/interactive/up_threshold_any_cpu_load

# Permissions for Audio
chown system.system /sys/devices/fe1af000.slim/es705-codec-gen0/keyword_grammar_path
chown system.system /sys/devices/fe1af000.slim/es705-codec-gen0/keyword_net_path
chown system.system /sys/devices/fe1af000.slim/es704-codec-gen0/keyword_grammar_path
chown system.system /sys/devices/fe1af000.slim/es704-codec-gen0/keyword_net_path

#Set Mode Change Condition
echo 340 > /sys/devices/system/cpu/cpufreq/interactive/multi_enter_load
echo 99000 > /sys/devices/system/cpu/cpufreq/interactive/multi_enter_time
echo 90 > /sys/devices/system/cpu/cpufreq/interactive/multi_exit_load
echo 299000 > /sys/devices/system/cpu/cpufreq/interactive/multi_exit_time
echo 90 > /sys/devices/system/cpu/cpufreq/interactive/single_enter_load
echo 199000 > /sys/devices/system/cpu/cpufreq/interactive/single_enter_time
echo 60 > /sys/devices/system/cpu/cpufreq/interactive/single_exit_load
echo 99000 > /sys/devices/system/cpu/cpufreq/interactive/single_exit_time

#Default interactive governor parameters
echo 0 > /sys/devices/system/cpu/cpufreq/interactive/param_index
echo 1 > /sys/devices/system/cpu/cpufreq/interactive/io_is_busy
echo 20000 > /sys/devices/system/cpu/cpufreq/interactive/timer_rate
echo 20000 > /sys/devices/system/cpu/cpufreq/interactive/timer_slack
echo 40000 > /sys/devices/system/cpu/cpufreq/interactive/min_sample_time
echo 1190400 > /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq
echo 99 > /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load
echo "20000 1400000:80000 1500000:40000 1700000:20000" > /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay
echo "85 1400000:90 1700000:95" > /sys/devices/system/cpu/cpufreq/interactive/target_loads
echo 100000 > /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor
#Single Mode Parameter
echo 1 > /sys/devices/system/cpu/cpufreq/interactive/param_index
echo 20000 > /sys/devices/system/cpu/cpufreq/interactive/timer_rate
echo 59000 > /sys/devices/system/cpu/cpufreq/interactive/min_sample_time
echo 1497600 > /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq
echo 95 > /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load
echo 19000 > /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay
echo "60 800000:65 1400000:65 1700000:75" > /sys/devices/system/cpu/cpufreq/interactive/target_loads
echo 150000 > /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor
#Multi Mode Parameter
echo 2 > /sys/devices/system/cpu/cpufreq/interactive/param_index
echo 20000 > /sys/devices/system/cpu/cpufreq/interactive/timer_rate
echo 79000 > /sys/devices/system/cpu/cpufreq/interactive/min_sample_time
echo 1728000 > /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq
echo 90 > /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load
echo 19000 > /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay
echo "50 800000:60 1400000:65" > /sys/devices/system/cpu/cpufreq/interactive/target_loads
echo 200000 > /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor
#Single & Multi Mode Parameter
echo 3 > /sys/devices/system/cpu/cpufreq/interactive/param_index
echo 20000 > /sys/devices/system/cpu/cpufreq/interactive/timer_rate
echo 99000 > /sys/devices/system/cpu/cpufreq/interactive/min_sample_time
echo 1958400 > /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq
echo 85 > /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load
echo 19000 > /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay
echo "50 1400000:60" > /sys/devices/system/cpu/cpufreq/interactive/target_loads
echo 300000 > /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor
echo 0 > /sys/devices/system/cpu/cpufreq/interactive/param_index

# Change cpu-boost sysfs permission
chown -h system.system /sys/module/cpu_boost/parameters/sync_threshold
chown -h system.system /sys/module/cpu_boost/parameters/boost_ms
chmod -h 0660 /sys/module/cpu_boost/parameters/sync_threshold
chmod -h 0660 /sys/module/cpu_boost/parameters/boost_ms

# Change cpubw sysfs permission
chown radio.system /sys/class/devfreq/qcom,cpubw*/available_frequencies
chown radio.system /sys/class/devfreq/qcom,cpubw*/available_governors
chown radio.system /sys/class/devfreq/qcom,cpubw*/governor
chown radio.system /sys/class/devfreq/qcom,cpubw*/max_freq
chown radio.system /sys/class/devfreq/qcom,cpubw*/min_freq
chown -h system.system /sys/class/devfreq/qcom,cpubw*/cpubw_hwmon/guard_band_mbps
chown -h system.system /sys/class/devfreq/qcom,cpubw*/cpubw_hwmon/io_percent
chmod -h 0664 /sys/class/devfreq/qcom,cpubw*/available_frequencies
chmod -h 0664 /sys/class/devfreq/qcom,cpubw*/available_governors
chmod -h 0664 /sys/class/devfreq/qcom,cpubw*/governor
chmod -h 0664 /sys/class/devfreq/qcom,cpubw*/max_freq
chmod -h 0664 /sys/class/devfreq/qcom,cpubw*/min_freq
chmod -h 0660 /sys/class/devfreq/qcom,cpubw*/cpubw_hwmon/guard_band_mbps
chmod -h 0660 /sys/class/devfreq/qcom,cpubw*/cpubw_hwmon/io_percent

# Change PM debug parameters permission
chown -h radio.system /sys/module/qpnp_power_on/parameters/wake_enabled
chown -h radio.system /sys/module/qpnp_power_on/parameters/reset_enabled
chown -h radio.system /sys/module/qpnp_int/parameters/debug_mask
chown -h radio.system /sys/module/lpm_levels/parameters/secdebug
chmod -h 664 /sys/module/qpnp_power_on/parameters/wake_enabled
chmod -h 664 /sys/module/qpnp_power_on/parameters/reset_enabled
chmod -h 664 /sys/module/qpnp_int/parameters/debug_mask
chmod -h 664 /sys/module/lpm_levels/parameters/secdebug

# Volume down key(connect to PMIC RESIN) wakeup enable/disable
chown -h radio.system /sys/power/volkey_wakeup
chmod -h 664 /sys/power/volkey_wakeup
echo 0 > /sys/power/volkey_wakeup

chown -h system /sys/devices/system/cpu/cpufreq/ondemand/sampling_rate
chown -h system /sys/devices/system/cpu/cpufreq/ondemand/sampling_down_factor
chown -h system /sys/devices/system/cpu/cpufreq/ondemand/io_is_busy

# Post-setup services
rm /data/system/default_values
start mpdecision
echo 512 > /sys/block/mmcblk0/bdi/read_ahead_kb
echo 512 > /sys/block/sda/bdi/read_ahead_kb
echo 512 > /sys/block/sdb/bdi/read_ahead_kb
echo 512 > /sys/block/sdc/bdi/read_ahead_kb
echo 512 > /sys/block/sdd/bdi/read_ahead_kb
echo 512 > /sys/block/sde/bdi/read_ahead_kb
echo 512 > /sys/block/sdf/bdi/read_ahead_kb
echo 512 > /sys/block/sdg/bdi/read_ahead_kb
echo 512 > /sys/block/sdh/bdi/read_ahead_kb

