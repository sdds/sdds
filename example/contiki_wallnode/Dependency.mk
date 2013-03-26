DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/DoorSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/MovementSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffSwitch-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/HumiditySensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/DimmableLight-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightRegulationFunctionality-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffFunctionality-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/SimpleLamp-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/ToggleFunctionality-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/WindowSensor-ds.o


APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_WallnodeApp.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Light.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Temperature.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Movement.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Humidity.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_DimmerLamp.o


DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-AMN31112.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-TSL2561.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-twi.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-LED.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-ds18x20.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-crc8.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-ow.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-AMN31112.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-GammaCorrection.o
