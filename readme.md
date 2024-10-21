# SMSL Remote for extern amp control kit

## **Remotr function**

### A mode
* Source : Enter/Leave amp servo angle modify resolution config mode
    * Up : Resolution decrease
    * Down : Resolution increase
* Function : Enter/Leave amp volume gear motor relay active level config mode
    * Up : HIGH active
    * Down : LOW active

### B mode
* Enter : Save current config
* Mute : reset to default config
* Source : Enter/Leave amp **ON** servo angle config mode
    * Up : Angle increase
    * Down : Angle decrease
* Function : Enter/Leave amp **OFF** servo angle config mode
    * Up : Angle increase
    * Down : Angle decrease
* Power : Enter/Leave amp volume gear motor rolling direction config mode
    * Up : Clockwise as volume up
    * Down : Clockwise as volume down
* Left : Enter/Leave amp volume gear motor rolling time config mode
    * Up : Rolling time increase
    * Down : Rolling time decrease



### **C mode** (General usage)
* **Power** : Power on DAC(su-9) and AMP (Servo moter on/off)
* ~~**Up** : volum up (Gear motor clockwise)~~
* ~~**Down** : volum down (Gear motor counterclockwise)~~

---


## **ESP32 pin**
|GPIO|Function|note|
|:---:|:---|:---|
|02|Servo motor singal|||
|12|IR receive signal|||
|15|gear motor clockwise|relay 0, 3||
|13|gear motor counterclockwise|relay 1, 2||
