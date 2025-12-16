# VARTA-IoT Simulations

## Overview

This folder contains MATLAB/Simulink simulations for testing and validating the IoT soil monitoring system under realistic environmental conditions. The simulations model the Solan/Nauni field site in Himachal Pradesh, India, and provide virtual sensor data streams for system validation before field deployment.

## Purpose

- **System Validation**: Test sensor algorithms and control logic without hardware
- **Climate Modeling**: Simulate realistic environmental conditions for the Himalayan region
- **Algorithm Development**: Develop and tune data processing algorithms
- **Performance Prediction**: Estimate system behavior under various environmental scenarios
- **Field Testing Baseline**: Establish baseline expectations before actual field deployment

## Folder Structure

```
simulations/
├── code/                          # MATLAB functions and initialization scripts
│   ├── init_nauni.m              # Site-specific initialization and parameters
│   └── matlab_func.m             # Diurnal cycle generation function
├── model/                         # Simulink models
│   └── nauni_fieldtesting.slx    # Main Simulink simulation model
└── README.md                      # This file
```

## Site Configuration (Solan/Nauni)

### Geographic Information
- **Latitude**: 30.85° N
- **Longitude**: 77.18° E
- **Altitude**: 1350 m above sea level
- **Region**: Himachal Pradesh, India

### Typical November Climate (Baseline)

| Parameter | Value | Notes |
|-----------|-------|-------|
| **Temperature Mean** | 16°C | Moderate autumn conditions |
| **Temperature Swing** | ±7°C | Diurnal variation (5°C to 23°C) |
| **Humidity Mean** | 65% RH | Post-monsoon transition |
| **Humidity Swing** | ±20% RH | 45% to 85% RH daily |
| **Initial Soil Moisture** | 28% VWC | Moderate soil water content |
| **Soil Moisture Range** | 8-45% VWC | Min (dry) to max (saturated) |

### Environmental Processes

#### Temperature Cycle
- **Minimum**: ~05:00 (pre-dawn)
- **Maximum**: ~15:00 (mid-afternoon)
- **Model**: Sinusoidal diurnal pattern
- **Equation**: $T(t) = T_{mean} + T_{amp} \sin\left(\frac{2\pi(h-5)}{24}\right)$
  - Where $h$ = hour of day (0-24)

#### Relative Humidity Cycle
- **Maximum**: Pre-dawn (~00:00-06:00)
- **Minimum**: Mid-day (~12:00-15:00)
- **Anti-phase**: RH rises as temperature falls
- **Model**: Sinusoidal pattern offset from temperature
- **Equation**: $RH(t) = RH_{mean} + RH_{amp} \sin\left(\frac{2\pi(h+7)}{24}\right)$

#### Soil Moisture Dynamics
- **Evaporation**: 0.2% VWC/hour under normal conditions
- **Irrigation Events**: 6% VWC added every 36 hours
- **Constraints**: Moisture bounded between 8% (dry) and 45% (saturated)
- **Rate**: Evaporation rate = 0.2 ÷ 3600 %VWC/hour

## Sensor Models

### Sensor Dynamics

All sensors are modeled with **first-order lag** (low-pass filter) to simulate realistic sensor response characteristics:

- **Time Constant (τ)**: 2 seconds
- **Sample Time (Ts)**: 1 second
- **Transfer Function**: $\frac{1}{1 + \tau s}$
- **Discrete Implementation**: $a = e^{-T_s/\tau}$

This creates a realistic 2-second response delay in sensor readings.

### Sensor Noise

Sensor measurements include realistic noise (1-sigma values):

| Sensor | Noise Level | Unit |
|--------|-------------|------|
| **Temperature** | ±0.1 | °C |
| **Humidity** | ±0.5 | %RH |
| **Soil Moisture** | ±0.7 | %VWC |

## MATLAB Scripts

### `init_nauni.m` - Simulation Initialization

**Purpose**: Define all site parameters, simulation settings, and environmental conditions

**Key Variables**:
```matlab
% Site location
site.lat = 30.85;          % Latitude (°N)
site.lon = 77.18;          % Longitude (°E)
site.alt_m = 1350;         % Altitude (meters)

% Simulation timing
Ts = 1;                    % Sample time (seconds)
simDays = 3;               % Simulation duration (days)
StopTime = simDays*86400;  % Total simulation time (seconds)

% Environmental conditions
T_mean = 16;               % Mean temperature (°C)
T_amp = 7;                 % Temperature amplitude (°C)
RH_mean = 65;              % Mean relative humidity (%)
RH_amp = 20;               % Humidity amplitude (%)

% Sensor characteristics
tau_s = 2;                 % Sensor time constant (seconds)
a = exp(-Ts/tau_s);       % First-order lag coefficient

% Noise specifications
noise.T = 0.1;             % Temperature noise (1-sigma, °C)
noise.RH = 0.5;            % Humidity noise (1-sigma, %RH)
noise.VWC = 0.7;           % VWC noise (1-sigma, %VWC)

% Soil moisture model
VWC_init = 28;             % Initial volumetric water content (%)
VWC_min = 8;               % Minimum VWC (dry soil)
VWC_max = 45;              % Maximum VWC (saturated)
evap_per_hour = 0.2;       % Hourly evaporation rate (%VWC/hour)
rain_pulse = 6;            % Water added per irrigation (%)
rain_every_hours = 36;     % Irrigation interval (hours)
evap_rate = evap_per_hour/3600;  % Evaporation rate (%VWC/second)
```

**Usage**:
```matlab
% Run at start of Simulink session
init_nauni;  % Loads all parameters into MATLAB workspace
```

**Customization Points**:
- Adjust `simDays` to change simulation duration
- Modify `T_mean`, `T_amp`, `RH_mean`, `RH_amp` for different seasonal conditions
- Change `VWC_init`, `evap_per_hour`, `rain_pulse` for different moisture scenarios
- Tune `noise.*` values to test sensor robustness

### `matlab_func.m` - Diurnal Cycle Generator

**Purpose**: Generate realistic hourly temperature and humidity patterns

**Function Signature**:
```matlab
[T_raw, RH_raw] = diurnal_sht40(t, T_mean, T_amp, RH_mean, RH_amp)
```

**Parameters**:
- `t`: Time in seconds
- `T_mean`: Mean temperature (°C)
- `T_amp`: Temperature amplitude (°C)
- `RH_mean`: Mean relative humidity (%)
- `RH_amp`: Humidity amplitude (%)

**Outputs**:
- `T_raw`: Unfiltered temperature value (°C)
- `RH_raw`: Unfiltered relative humidity value (%)

**Behavior**:
- Converts time `t` to hour of day: $h = \text{mod}(t/3600, 24)$
- Generates smooth sinusoidal patterns with physically realistic phase relationships
- Temperature peaks ~3 hours after humidity minimum
- Can be called at each simulation timestep to generate continuous sensor data

**Example Usage** (in Simulink):
```matlab
[T, RH] = diurnal_sht40(t, 16, 7, 65, 20);  % Using values from init_nauni
```

## Simulink Model

### `nauni_fieldtesting.slx`

**Model Overview**: 
The main Simulink model integrates all simulation components into a unified environment testing platform.

**Expected Components** (typical structure):
1. **Environmental Generator Block**: Creates temperature, humidity, and moisture profiles
2. **Sensor Blocks**: Implements sensor dynamics (lag), noise injection, and quantization
3. **Data Logging**: Records all sensor outputs to workspace variables
4. **Visualization**: Real-time scopes for monitoring during simulation
5. **Analysis Tools**: Export capabilities for post-processing in MATLAB

**Typical Workflow**:
```matlab
% 1. Initialize simulation parameters
init_nauni;

% 2. Open Simulink model
open('nauni_fieldtesting.slx');

% 3. Run simulation
sim('nauni_fieldtesting.slx', struct('StopTime', num2str(StopTime)));

% 4. Analyze results in workspace
```

## Running Simulations

### Prerequisites
- MATLAB R2020a or later
- Simulink
- Signal Processing Toolbox (optional, for advanced analysis)

### Basic Workflow

1. **Open MATLAB** in the simulations folder or add it to path:
   ```matlab
   addpath(genpath('code/'));  % Add code folder
   ```

2. **Initialize parameters**:
   ```matlab
   init_nauni;
   ```

3. **Open and run Simulink model**:
   ```matlab
   open('model/nauni_fieldtesting.slx');
   sim('model/nauni_fieldtesting.slx');
   ```

4. **Access simulation results** (check model for output variable names):
   ```matlab
   % Example: plot temperature over time
   plot(T_out);
   xlabel('Time (s)');
   ylabel('Temperature (°C)');
   title('Simulated Temperature - Solan Site');
   grid on;
   ```

### Advanced Usage

#### Change Simulation Duration
```matlab
init_nauni;
StopTime = 7 * 86400;  % Change to 7 days
sim('model/nauni_fieldtesting.slx');
```

#### Test Different Environmental Conditions
```matlab
init_nauni;
T_mean = 12;      % Winter conditions
T_amp = 10;       % Larger daily swing
RH_mean = 80;     % Higher humidity
RH_amp = 15;
sim('model/nauni_fieldtesting.slx');
```

#### Increase Sensor Noise for Robustness Testing
```matlab
init_nauni;
noise.T = 0.5;    % 5x normal noise
noise.RH = 2.5;
noise.VWC = 3.5;
sim('model/nauni_fieldtesting.slx');
```

#### Disable Irrigation to Test Drought Conditions
```matlab
init_nauni;
rain_pulse = 0;   % No irrigation
rain_every_hours = inf;  % Prevent any rain events
sim('model/nauni_fieldtesting.slx');
```

## Output Analysis

### Expected Output Variables (typical naming)

After running the simulation, the workspace should contain:

| Variable | Description | Unit |
|----------|-------------|------|
| `tout` | Time vector | seconds |
| `T_raw` | Raw temperature (noisy) | °C |
| `T_filtered` | Filtered temperature | °C |
| `RH_raw` | Raw humidity (noisy) | %RH |
| `RH_filtered` | Filtered humidity | %RH |
| `VWC` | Volumetric water content | % |
| `T_sensor` | Simulated SHT30 output | °C |
| `RH_sensor` | Simulated humidity sensor | %RH |

### Common Analysis Tasks

#### 1. Verify Diurnal Patterns
```matlab
figure;
subplot(2,1,1);
plot(tout/3600, T_filtered);  % Convert seconds to hours
xlabel('Time (hours)');
ylabel('Temperature (°C)');
title('Temperature Diurnal Cycle');
grid on;

subplot(2,1,2);
plot(tout/3600, RH_filtered);
xlabel('Time (hours)');
ylabel('Relative Humidity (%)');
title('Humidity Diurnal Cycle');
grid on;
```

#### 2. Analyze Soil Moisture Evolution
```matlab
figure;
plot(tout/3600, VWC);
xlabel('Time (hours)');
ylabel('Volumetric Water Content (%)');
title('Soil Moisture Evolution');
grid on;
hold on;
yline(VWC_min, 'r--', 'Min');
yline(VWC_max, 'b--', 'Max');
legend('VWC', 'Minimum', 'Maximum');
```

#### 3. Assess Sensor Noise Impact
```matlab
figure;
subplot(3,1,1);
plot(tout/3600, T_raw, 'b', tout/3600, T_filtered, 'r');
legend('Raw (noisy)', 'Filtered');
ylabel('Temperature (°C)');
title('Sensor Noise - Temperature');
grid on;

subplot(3,1,2);
plot(tout/3600, RH_raw, 'b', tout/3600, RH_filtered, 'r');
legend('Raw (noisy)', 'Filtered');
ylabel('Humidity (%)');
title('Sensor Noise - Humidity');
grid on;

subplot(3,1,3);
plot(tout/3600, T_raw - T_filtered);
ylabel('Noise (°C)');
xlabel('Time (hours)');
title('Estimated Noise Component');
grid on;
```

## Extending the Simulation

### Adding New Sensors

To add a new sensor (e.g., soil pH, NPK):

1. **Create initialization variables in `init_nauni.m`**:
   ```matlab
   pH_init = 6.5;
   pH_noise = 0.1;
   ```

2. **Add generation function in `matlab_func.m`**:
   ```matlab
   function pH_out = generate_pH(t)
      % Slight diurnal variation in pH
      pH_out = pH_init + 0.2 * sin(2*pi*t/86400);
   end
   ```

3. **Add blocks in Simulink model** for the new sensor

### Modifying Environmental Models

To add atmospheric pressure, wind speed, or soil depth effects:

1. Expand `init_nauni.m` with new parameters
2. Create new generation functions in `matlab_func.m`
3. Update Simulink model to call these functions

### Seasonal Variations

Create seasonal variants:
```matlab
% nauni_winter.m
init_nauni;
T_mean = 8;
T_amp = 12;
RH_mean = 85;
simDays = 7;
StopTime = simDays * 86400;

% nauni_summer.m
init_nauni;
T_mean = 28;
T_amp = 15;
RH_mean = 45;
simDays = 7;
StopTime = simDays * 86400;
```

## Troubleshooting

### Common Issues

**1. Model won't run - missing variables**
- Solution: Ensure `init_nauni` is run before simulation
- Check variable names match between `.m` files and Simulink model

**2. Sensor outputs are constant/flat**
- Check that diurnal generation functions are connected
- Verify time constant values are reasonable
- Ensure noise levels are non-zero for testing

**3. Soil moisture doesn't change**
- Verify `evap_rate` calculation is correct
- Check irrigation logic is enabled
- Monitor `StopTime` isn't too short

**4. Unrealistic values**
- Review parameter ranges in `init_nauni.m`
- Verify sensor saturation limits
- Check units consistency (seconds vs hours)

### Debugging Tips

```matlab
% Display current simulation parameters
disp(site);
disp(noise);
fprintf('Simulation duration: %.1f hours\n', StopTime/3600);

% Test diurnal function at specific times
hr = 5;  % 5 AM
t = hr * 3600;
[T, RH] = diurnal_sht40(t, T_mean, T_amp, RH_mean, RH_amp);
fprintf('At %d:00 -> T=%.1f°C, RH=%.1f%%\n', hr, T, RH);
```

## Field Testing Integration

Once validation simulations are successful:

1. **Compare simulated vs. real data**: Deploy system at Solan site and compare
2. **Calibrate models**: Adjust simulation parameters based on field observations
3. **Validate algorithms**: Test control logic and MQTT publishing under simulated load
4. **Load testing**: Run extended simulations to identify memory leaks or timing issues

## References

### Related Documentation
- **Main README**: See `../README.md` for hardware and firmware details
- **Main Code**: See `../src/main.cpp` for sensor integration

### MATLAB/Simulink Resources
- [Simulink Documentation](https://www.mathworks.com/help/simulink/)
- [First-Order Lag Model](https://www.mathworks.com/help/control/ref/tf.html)
- [Environmental Sensors](https://www.mathworks.com/help/physmod/simscape/sensors.html)

## License
The Similations are part of the VARTA project Funded through a Grant by Gram Disha Trust

