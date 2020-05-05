"""import important library"""
import serial
import time
import csv

"""port in macbook connected to arduino"""
ser = serial.Serial('/dev/cu.usbmodem144101') 
ser.flushInput()

""" get data from serial port and store in a csv file """
while True:
    try:
        ser_bytes = ser.readline() """read data line by line """
        decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8")) """decoding"""
        print(decoded_bytes)
        with open("test_data.csv","a") as f: """store in a csv file"""
            writer = csv.writer(f,delimiter=",")
            writer.writerow([time.time(),decoded_bytes])
    except:
        print("Keyboard Interrupt")
        break



"""arduino = serial.Serial("/dev/cu.usbmodem144101",timeout=1,baudrate=9600)"""