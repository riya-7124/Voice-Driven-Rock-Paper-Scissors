# get_voice_input.py
import speech_recognition as sr
import sys

def get_user_choice():
    recognizer = sr.Recognizer()
    max_attempts = 3
    with sr.Microphone() as source:
        for attempt in range(max_attempts):
            try:
                # Adjust for ambient noise before each attempt
                print(f"Adjusting for ambient noise (attempt {attempt + 1})...", file=sys.stderr)
                recognizer.adjust_for_ambient_noise(source, duration=1)
                print("Say rock, paper, or scissors:", file=sys.stderr)
                audio = recognizer.listen(source, phrase_time_limit=7)
                print("Processing audio...", file=sys.stderr)
                text = recognizer.recognize_google(audio).lower()
                print(f"Recognized text: {text}", file=sys.stderr)
                if text in ["rock", "paper", "scissors"]:
                    print(text)  # Output only the valid choice to stdout
                    return
                else:
                    print(f"Unrecognized word: {text}", file=sys.stderr)
                    print("invalid")
                    return
            except sr.UnknownValueError:
                print(f"Attempt {attempt + 1}/{max_attempts}: Could not understand audio", file=sys.stderr)
                if attempt < max_attempts - 1:
                    print("Please try again...", file=sys.stderr)
                else:
                    print("invalid")
                    return
            except sr.RequestError as e:
                print(f"Attempt {attempt + 1}/{max_attempts}: Error with speech recognition service: {e}", file=sys.stderr)
                print("invalid")
                return
            except Exception as e:
                print(f"Unexpected error: {e}", file=sys.stderr)
                print("invalid")
                return

if __name__ == "__main__":
    get_user_choice()