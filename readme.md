# SMSL Remote for extern amp control kit

## **Remotr function**

### A mode
* Not use for now

### B mode
* Enter : Save current config
* Mute : reset to default config
* Source : Enter/Leave amp **ON** servo angle config mode
    * Up : Angle increase
    * Down : Angle decrease
* Function : Enter/Leave amp **OFF** servo angle config mode
    * Up : Angle increase
    * Down : Angle decrease


### **C mode** (General usage)
* **Power** : Power on DAC(su-9) and AMP (Servo moter on/off)
* ~~**Up** : volum up (Geat motor clockwise)~~
* ~~**Down** : volum down (Geat motor counterclockwise)~~

---


## **ESP32 pin**
|GPIO|Function|note|
|:---:|:---|:---|
|02|Servo motor singal|||
|12|IR receive signal|||
|~~15~~|~~gear motor clockwise~~|~~relay 0, 3~~||
|~~13~~|~~gear motor counterclockwise~~|~~relay 1, 2~~||
