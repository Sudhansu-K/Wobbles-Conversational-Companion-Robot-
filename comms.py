import websocket
import subprocess
import keyboard
import time
import sys
import os

stream_process=subprocess.Popen([sys.executable,'speechstream.py'])
ai_process=subprocess.Popen([sys.executable,'bubbles.py']) 

curr_face=""

ws=websocket.WebSocket()
ws.connect("ws://10.53.7.11")
#192.168.175.176  10.53.7.11
#192.168.175.11 - breadboard
print("Connected to body")
logfilepath="elog.txt"
initial_mtime=os.path.getmtime(logfilepath)
current_mtime=os.path.getmtime(logfilepath)

if ws.connected:
    print("Control")
    while ws.connected:
        
        if keyboard.is_pressed('w'): 
            print('w')
            ws.send('w')
            while keyboard.is_pressed('w'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('a'): 
            print('a')
            ws.send('a')
            while keyboard.is_pressed('a'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('s'): 
            print('s')
            ws.send('s')
            while keyboard.is_pressed('s'):
                    time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('d'): 
            print('d')
            ws.send('d')
            while keyboard.is_pressed('d'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('q'): 
            print('q')
            ws.send('q')
            while keyboard.is_pressed('q'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('e'): 
            print('e')
            ws.send('e')
            while keyboard.is_pressed('e'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('r'): 
            print('r')
            ws.send('r')
            while keyboard.is_pressed('r'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('f'): 
            print('f')
            ws.send('f')
            while keyboard.is_pressed('f'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('x'):
            print("quiting...")
            ws.close()
            stream_process.terminate()
            ai_process.terminate()
            break
        if keyboard.is_pressed('t'):
            print('t')
            ws.send('t')
            while True:
                    if current_mtime != initial_mtime:
                        print("EXPRESSION UPDATED")
                        initial_mtime=current_mtime
                        time.sleep(0.1)
                        with open(logfilepath,"r") as f:
                            curr_face=f.readline().strip()
                        print(curr_face)
                        ws.send('!')
                        ws.send(curr_face)
                        current_mtime=os.path.getmtime(logfilepath)
                        initial_mtime=current_mtime
                        break
                    current_mtime=os.path.getmtime(logfilepath)
            ws.send('!')
            ws.send('T')
            while keyboard.is_pressed('t'):
                time.sleep(0.01)
            ws.send('!')
               
        if keyboard.is_pressed('1'):
            print('1')
            ws.send('1')
            while keyboard.is_pressed('1'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('2'):
            print('2')
            ws.send('2')
            while keyboard.is_pressed('2'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('3'):
            print('3')
            ws.send('3')
            while keyboard.is_pressed('3'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('4'):
            print('4')
            ws.send('4')
            while keyboard.is_pressed('4'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('5'):
            print('5')
            ws.send('5')
            while keyboard.is_pressed('5'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('6'):
            print('6')
            ws.send('6')
            while keyboard.is_pressed('6'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('7'):
            print('7')
            ws.send('7')
            while keyboard.is_pressed('7'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('8'):
            print('8')
            ws.send('8')
            while keyboard.is_pressed('8'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('9'):
            print('9')
            ws.send('9')
            while keyboard.is_pressed('9'):
                time.sleep(0.01)
            ws.send('!')
        if keyboard.is_pressed('0'):
            print('0')
            ws.send('0')
            while keyboard.is_pressed('0'):
                time.sleep(0.01)
            ws.send('!')


