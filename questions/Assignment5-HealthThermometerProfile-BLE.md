Please include your answers to the questions below with your submission, entering into the space below each question
See [Mastering Markdown](https://guides.github.com/features/mastering-markdown/) for github markdown formatting if desired.

*Be sure to take measurements with logging disabled to ensure your logging logic is not impacting current/time measurements.*

*Please include screenshots of the profiler window detailing each current measurement captured.  See the file Instructions to add screenshots in assignment.docx in the ECEN 5823 Student Public Folder.*

1. Provide screen shot verifying the Advertising period matches the values required for the assignment.
   <br>Screenshot:  250.80 ms (Almost 250 ms matches the required value)
   ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/1959e020-76bb-40d5-b93c-1aeeb8581ac3)


2. What is the average current between advertisements (Don't measure an interval with a LETIMER UF event)?
   Answer:
   <br>Screenshot:  95.72 uA
  ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/3d335c63-21aa-418f-9d06-40cc2f59ee26)
 

3. What is the peak current of an advertisement? 
   Answer:
   <br>Screenshot:  3.78 mA
   ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/2403d737-0dd6-4c05-a595-4c1c1bf67c3b)
  

4. Provide screen shot showing the connection interval settings. Do they match the values you set in your slave(server) code or the master's(client) values?.
   <br>Screenshot:
   <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/722501fd-3174-4614-9da0-14eb3b293404)
   <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/964bb9cf-da47-4c55-9082-c68de0e6e8b1)

   <br> The connection parameters does not match with what the server wants it to be / setting in the code for server.
   <br>The connection interval matches with the log from the connection parameters. It log reflects the values that client wants and hence configured.
   <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/373d628f-3d00-4759-83b2-3b5766e2ee10)

   <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/58a73eca-8bc3-4ead-9be7-90d99b75e062)

6. What is the average current between connection intervals (Don't measure an interval with a LETIMER UF event)?
   Answer:
   <br>Screenshot: 858.52 uA 
   ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/5da16406-df55-4bb5-be60-cc55d444624a)
 
7. If possible, provide screen shot verifying the slave latency matches what was reported when you logged the values from event = gecko_evt_le_connection_parameters_id. 
   <br>Screenshot:
  <br> It represents the number of connection intervals that the peripheral is allowed to skip before it responds to a communication
   request from the central device. Here it responds immediately in the next connection so the latency is 0, hence verified.

  <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/e8cd6d4a-378d-43f5-a9b3-aad06f1b288f)

  <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/dc07dbdf-72a8-4e3f-958b-0abac802a41b)

  <br> ![image](https://github.com/CU-ECEN-5823/ecen5823-assignment5-adna4700/assets/112426457/9e7372b4-3d3d-4172-839e-37df262f2a46)


