from flask import Flask, Response
import soundcard as sc
import numpy as np
import subprocess
import threading

app = Flask(__name__)

SAMPLE_RATE = 48000
CHANNELS = 2
CHUNK_DURATION = 0.2  # seconds
CHUNK_SIZE = int(SAMPLE_RATE * CHUNK_DURATION)

# Get the virtual mic from VB-Cable
mic = sc.get_microphone('CABLE Output (VB-Audio Virtual Cable)', include_loopback=True)

# Shared list of clients to stream to
clients = []

def audio_stream():
    """Capture raw audio and stream to ffmpeg for each connected client."""
    with mic.recorder(samplerate=SAMPLE_RATE, channels=CHANNELS) as recorder:
        print(" Audio capture started...")
        while True:
            data = recorder.record(numframes=CHUNK_SIZE)
            raw_bytes = (data * 32767).astype(np.int16).tobytes()
            # Broadcast to all connected ffmpeg processes
            for ffmpeg_proc in clients.copy():
                try:
                    ffmpeg_proc.stdin.write(raw_bytes)
                except Exception as e:
                    print(f"X Client disconnected: {e}")
                    try:
                        ffmpeg_proc.stdin.close()
                        clients.remove(ffmpeg_proc)
                    except:
                        pass

@app.route('/live.mp3')
def live_mp3():

    """Return a streaming MP3 response."""
    # Start a subprocess to encode PCM to MP3
    ffmpeg_proc = subprocess.Popen(
        ['ffmpeg', '-f', 's16le', '-ar', str(SAMPLE_RATE), '-ac', str(CHANNELS),
         '-i', 'pipe:0', '-f', 'mp3', '-'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        bufsize=0
    )

    # Register this client
    clients.append(ffmpeg_proc)

    def generate():
        try:
            while True:
                data = ffmpeg_proc.stdout.read(1024)
                if not data:
                    break
                yield data
        except GeneratorExit:
            pass
        finally:
            print("Client stream ended.")
            try:
                ffmpeg_proc.stdin.close()
                ffmpeg_proc.stdout.close()
                ffmpeg_proc.kill()
                clients.remove(ffmpeg_proc)
            except:
                pass

    return Response(generate(), mimetype='audio/mpeg')

if __name__ == '__main__':
    # Start the audio thread
    threading.Thread(target=audio_stream, daemon=True).start()
    app.run(host="0.0.0.0", port=8000, threaded=True)
