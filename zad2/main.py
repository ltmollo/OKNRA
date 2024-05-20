import pandas as pd


data_string = """The total instructions executed are 528042, total cycles 178750

The total instructions executed are 4067422, total cycles 1199627

The total instructions executed are 13563604, total cycles 4009738

The total instructions executed are 31960586, total cycles 9414012

The total instructions executed are 62202370, total cycles 18244082

The total instructions executed are 107232952, total cycles 31135283

The total instructions executed are 169996340, total cycles 49259421

The total instructions executed are 253436546, total cycles 73993617

The total instructions executed are 360497523, total cycles 104110456

The total instructions executed are 494123311, total cycles 142572963

The total instructions executed are 657257933, total cycles 189893291

The total instructions executed are 852845329, total cycles 246202545

The total instructions executed are 1083829555, total cycles 312437107

The total instructions executed are 1353154650, total cycles 389379849

The total instructions executed are 1663764522, total cycles 481225044

The total instructions executed are 2018603038, total cycles 584976046

The total instructions executed are 2420614557, total cycles 702525638

The total instructions executed are 2872742797, total cycles 833832371

The total instructions executed are 3377931992, total cycles 984636814

The total instructions executed are 3939125844, total cycles 1152511810

The total instructions executed are 4559268554, total cycles 1327248617

The total instructions executed are 5241303902, total cycles 1519207178

The total instructions executed are 5988176123, total cycles 1736254516

The total instructions executed are 6802829111, total cycles 1965544337

The total instructions executed are 7688207125, total cycles 2298002472
"""

name = 'chol1'


instructions = {}
cycles = {}
words = data_string.split()
i = 40
for word in words:
    if word[0] in ['0','1','2','3','4','5','6','7','8','9']:
        if word[-1] == ',':
            word = word[0 : len(word) - 1]
            instructions[i] = int(word)
        else:
            cycles[i] = int(word)
            i += 40

# df = pd.DataFrame(list(instructions.items()), columns=['Rozmiar', 'chol1'])
# df.to_excel('instrukcje.xlsx', index=False)
#
# df2 = pd.DataFrame(list(cycles.items()), columns=['Rozmiar', 'chol1'])
# df2.to_excel('cykle.xlsx', index=False)

df = pd.read_excel('instrukcje.xlsx')
df[name] = list(instructions.values())
df.to_excel('instrukcje.xlsx', index=False)

df2 = pd.read_excel('cykle.xlsx')
df2[name] = list(cycles.values())
df2.to_excel('cykle.xlsx', index=False)


print("Dane zostały zapisane do pliku.")


print(instructions)
