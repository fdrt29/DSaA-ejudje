import sys


def calculate(half):
    bank = half
    res = []
    while bank:
        if bank % 2 == 0:
            res.append("dbl")
            bank >>= 1
        else:
            if bank == 1 or bank == 3:
                res.append("inc")
                bank -= 1
            else:
                if available_division_count(bank + 1) > available_division_count(bank - 1):
                    res.append("dec")
                    bank += 1
                else:
                    res.append("inc")
                    bank -= 1
    return res


def available_division_count(num: int):
    count = 0
    # свдиг - деление, когда единица упрется в правый край - нечетное
    while not num & 1:
        count += 1
        num >>= 1
    return count


def main():
    half = 0

    for line in sys.stdin:
        if line == '\n' or line == '':
            continue

        if line.strip('\n').isdigit():
            try:
                half = int(line.strip('\n'))
                break
            except ValueError:
                print("error")
                continue

        # Else
        print("error")

    print(*reversed(calculate(half)), sep='\n')


if __name__ == '__main__':
    main()
