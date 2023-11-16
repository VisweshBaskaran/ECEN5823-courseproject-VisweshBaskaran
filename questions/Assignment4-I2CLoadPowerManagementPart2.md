Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.* 

1. What is the average current per period?
   Answer:
   
   <br>Screenshot:  18.88 uA
      ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment4-adna4700/assets/112426457/29c6ebee-dcdf-4cfe-b904-c91190310869)


3. What is the average current when the Si7021 is Powered Off?
   Answer:
   <br>Screenshot:  8.83 uA
 ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment4-adna4700/assets/112426457/1c93be7a-2c02-4530-bcf5-4288fbd63964)


4. What is the average current when the Si7021 is Powered On?
   Answer:
   <br>Screenshot:  403.57 uA
   ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment4-adna4700/assets/112426457/347139a5-88bf-4811-a34b-0aba795207c8)
  

5. How long is the Si7021 Powered On for 1 temperature reading?
   Answer:
   <br>Screenshot:  98.60 ms
   ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment4-adna4700/assets/112426457/eafec923-c45e-4511-8be1-088cfbc4a9d5)
 

6. Compute what the total operating time of your design for assignment 4 would be in hours, assuming a 1000mAh battery power supply?
   Answer (in hours):
 Total operating time =  battery power/average current for a period
                      =   1000 mAh/0.01888 = 52966.1017 hrs
   
7. How has the power consumption performance of your design changed since the previous assignment?
   Answer:
   Yes, power consumption has drastically decreased in this assignment as compared to the previous one. 
Previous Power Consumption: 481.46 uW
Current Power Consumption: 62.90 uW
The reason behind this is the decrease in the current consumption when the Temperature sensor - Si7021; is due to the use of interrupts and the system sleeps all the other times except during I2C transactions. This current consumption was more in the previous assignment because of the polling as the system has to stay awake and poll the current status of the system to do the commands; where as in an interrupt-based system it sleeps until it is interrupted.


