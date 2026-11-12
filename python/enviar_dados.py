
import serial
import time

# Configuracoes da comunicacao
PORTA = "COM3"
VELOCIDADE = 9600


def conectar_arduino():
    try:
        arduino = serial.Serial(PORTA, VELOCIDADE, timeout=1)

        # Aguarda o Arduino inicializar
        time.sleep(2)

        print("Arduino conectado com sucesso!")

        return arduino

    except serial.SerialException as erro:
        print("Erro ao conectar com o Arduino:")
        print(erro)

        return None


def enviar_dados(arduino, carros, pessoas):
    mensagem = f"C={carros},P={pessoas}\n"

    arduino.write(mensagem.encode("utf-8"))

    print("\nDados enviados ao Arduino:")
    print(mensagem.strip())


def main():
    arduino = conectar_arduino()

    if arduino is None:
        return

    try:
        while True:
            print("\n===== SEMAFORO INTELIGENTE =====")

            carros = int(input("Quantidade de carros: "))
            pessoas = int(input("Quantidade de pedestres: "))

            if not (0 <= carros <= 20 and 0 <= pessoas <= 20):
                print("Informe valores entre 0 e 20.")
                continue

            enviar_dados(arduino, carros, pessoas)

            time.sleep(0.5)

            while arduino.in_waiting > 0:
                resposta = arduino.readline().decode(
                    "utf-8", errors="replace"
                ).strip()

                if resposta:
                    print("Arduino:", resposta)

    except ValueError:
        print("Digite apenas numeros inteiros.")

    except KeyboardInterrupt:
        print("\nEncerrando comunicacao...")

    finally:
        arduino.close()
        print("Conexao encerrada.")


if __name__ == "__main__":
    main()
