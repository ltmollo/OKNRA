import pandas as pd


data_string = """The total instructions executed are 112865, total cycles 42315

The total instructions executed are 575849, total cycles 157526

The total instructions executed are 1529545, total cycles 417569

The total instructions executed are 3109129, total cycles 930431

The total instructions executed are 5451426, total cycles 1669043

The total instructions executed are 8691610, total cycles 2713171

The total instructions executed are 12966507, total cycles 4091210

The total instructions executed are 18411293, total cycles 5928211

The total instructions executed are 25162792, total cycles 8102635

The total instructions executed are 33356176, total cycles 10829256

The total instructions executed are 43128275, total cycles 14085164

The total instructions executed are 54614260, total cycles 18222279

The total instructions executed are 67950972, total cycles 22782298

The total instructions executed are 83273643, total cycles 28007539

The total instructions executed are 100718899, total cycles 34441850

The total instructions executed are 120422082, total cycles 44857279

The total instructions executed are 142520098, total cycles 53259172

The total instructions executed are 167147983, total cycles 66024946

The total instructions executed are 194442544, total cycles 79749095

The total instructions executed are 224539018, total cycles 93231376

The total instructions executed are 257573861, total cycles 111710477

The total instructions executed are 293682889, total cycles 128989455

The total instructions executed are 333002824, total cycles 148255325

The total instructions executed are 375668208, total cycles 166268378

The total instructions executed are 421816552, total cycles 193858503
"""

name = 'chol6 + O2 +znver1'


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
