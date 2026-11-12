
def validar_dados(carros, pessoas):
    if not isinstance(carros, int) or not isinstance(pessoas, int):
        return False

    if carros < 0 or carros > 20:
        return False

    if pessoas < 0 or pessoas > 20:
        return False

    return True


def gerar_mensagem(carros, pessoas):
    return f"C={carros},P={pessoas}\n"


def main():
    print("================================")
    print(" TESTE DE COMUNICACAO - SEMAFORO")
    print("================================")

    while True:
        print("\nDigite os dados identificados pela IA.")
        print("Digite 'sair' para encerrar.")

        entrada_carros = input("Quantidade de carros: ")

        if entrada_carros.lower() == "sair":
            break

        entrada_pessoas = input("Quantidade de pedestres: ")

        if entrada_pessoas.lower() == "sair":
            break

        try:
            carros = int(entrada_carros)
            pessoas = int(entrada_pessoas)

            if not validar_dados(carros, pessoas):
                print("ERRO: informe valores entre 0 e 20.")
                continue

            mensagem = gerar_mensagem(carros, pessoas)

            print("\n--- RESULTADO DO TESTE ---")
            print(f"Carros identificados: {carros}")
            print(f"Pedestres identificados: {pessoas}")
            print(f"Mensagem que seria enviada ao Arduino: {mensagem.strip()}")

            if carros > pessoas:
                print("Prioridade esperada: CARROS")

            elif pessoas > carros:
                print("Prioridade esperada: PEDESTRES")

            else:
                print("Prioridade esperada: EMPATE (alternancia)")

            print("--------------------------")

        except ValueError:
            print("ERRO: digite apenas numeros inteiros.")

    print("\nTeste encerrado.")


if __name__ == "__main__":
    main()
