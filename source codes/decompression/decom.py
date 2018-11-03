import pandas as pd
from collections import namedtuple
import warnings
warnings.filterwarnings('ignore')


data = pd.read_csv('./waveform.csv')
data.head()

f = open('codes2gtk.txt', 'w')
data.loc[:,data.columns[3]] = data[data.columns[3]].apply(str)
vals = data[data.columns[3]].apply(int, args=(2,)).values
keep = vals[vals<5000]

col = 'TRIGGER'

compressed = data[col].values

dictionaryArray = [namedtuple("MyStruct", "prefix character") for i in range(2023)]

def decode(code):
    if code>999:
        character = dictionaryArray[code].character
        temp = decode(dictionaryArray[code].prefix)
    else:
        character = code
        temp = code
    output.append(character)
    f.write("%d " %(character))
    return temp


compressed = keep
curentCode = 0
dictionarySize = 2023
val = 2047 
tempPrefix = 0 
characterMoved = 0
previousCode = compressed[0]
output = []
output.append(previousCode)
index = len(compressed) - 1
nextCode = 1000
for i in range(index):
    print('iteration: ', i)
    curentCode = compressed[i + 1]
    if curentCode>2023:
        tempPrefix=curentCode & val
        characterMoved=curentCode>>11
        decode(tempPrefix)
        output.append(characterMoved)
        exit()
    print('current code: ', curentCode)
    print('previous code: ', previousCode)
    if curentCode >= nextCode:
        print('[-] first')
        firstChar = decode(previousCode)
        print('first char: ', firstChar)
        output.append(firstChar)
        f.write("%d " %(firstChar))

    else:
        print('[-] second')
        firstChar = decode(curentCode)
        print('first char: ', firstChar)

    if nextCode < dictionarySize:
        print('[-] third')
        dictionaryArray[nextCode].prefix = previousCode
        dictionaryArray[nextCode].character = firstChar
        nextCode += 1
    print('next code', nextCode)
    print(' ')
    previousCode = curentCode

f.close()