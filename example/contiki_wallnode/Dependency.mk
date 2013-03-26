DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/DoorSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/LightSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/MovementSensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/OnOffSwitch-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureAndHumiditySensor-ds.o
DATA_DEPEND_OBJS += $(SDDS_OBJDIR)/TemperatureSensor-ds.o

APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_WallnodeApp.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Light.o
APP_DEPEND_OBJS += $(SDDS_OBJDIR)/Wiedas_SensorApp_Temperature.o

DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-AMN31112.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-TSL2561.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-twi.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-LED.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-ds18x20.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-crc8.o
DRIVER_DEPEND_OBJS += $(SDDS_OBJDIR)/sdds-driver-$(SDDS_ARCH)-ow.o
