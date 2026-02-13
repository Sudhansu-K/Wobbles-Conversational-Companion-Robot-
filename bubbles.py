import sounddevice as sd
import speech_recognition as sr
import ollama
from RealtimeTTS import TextToAudioStream, PiperVoice,PiperEngine
from RealtimeSTT import AudioToTextRecorder
import keyboard,time
import dotenv
import os
dotenv.load_dotenv()
path_v=os.getenv(en_gb)
path_v_j=os.getenv(en_gb_j)
path_piper=os.getenv(piper_p)

voice_model= PiperVoice(
        model_file=path_v,
        config_file=path_v_j,
)

engine=PiperEngine(piper_path=path_piper,voice=voice_model,)
speech=TextToAudioStream(engine)
def Bot(prompt):
        i=0
        stream_chat=ollama.chat(model='wobblesv1',messages=[{'role': 'user','content':prompt}],stream=True)
        for chunk in stream_chat:
            content=chunk.get("message",{}).get("content","")
            if content:
                print(i)
                if i==0:
                    with open("elog.txt","w") as f:
                        
                        f.write(content)
                    f.close()
                    print(content,end="",flush=True)
                else: 
                    print(content,end="",flush=True)
                    yield content
            i=i+1

if __name__=="__main__":
    recorder = AudioToTextRecorder(language='en',model="small")
    print("ai is ready")
    while True:
            if keyboard.is_pressed('x'):
                engine.shutdown()
                recorder.shutdown()
                break

            if keyboard.is_pressed('t'):
                print("\n\nSpeak when ready")
                print(f"<<<{(user_text:=recorder.text())}>>>", end="",flush=True)
                while recorder.is_recording:
                    time.sleep(0.01)
                speech.feed(Bot(user_text))
                speech.play()
                if not speech.is_playing_flag:
                    print("voice stop")
                while keyboard.is_pressed('t'):
                    time.sleep(0.01)

            
        
            



    