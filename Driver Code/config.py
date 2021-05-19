'''
problem_size = {'trapezoidal': [10**x for x in range(1, 9)],
                'vector': [10**x for x in range(1, 9)],
                'matrix_multiplication': [2**x for x in range(4, 11)],
                'pi_using_series': [10**x for x in range(1, 9)],
                'divide_and_conquer': [10**x for x in range(1, 9)] + [int(0.5*10**x) for x in range(5,9)],
                'image_warping': [2**x for x in range(3, 11)],
                'median_filtering': [2**x for x in range(3, 11)],
                'monte_carlo': [10**x for x in range(1, 9)] + [int(0.5*10**x) for x in range(5,9)] ,
                'prefix_sum': [10**x for x in range(1, 9)],
                'reduction': [10**x for x in range(1, 9)] + [int(0.5*10**x) for x in range(5,9)] ,
                'filter': [10**x for x in range(1, 9)]}
'''

#first part keep runs=2
#prob=['str1_256x256.txt', 'str1_512x512.txt','str5_256x256.txt', 'str5_512x512.txt','str9_256x256.txt', 'str9_512x512.txt']


#second part keep runs=1
#prob=['str1_1024x1024.txt', 'str5_1024x1024.txt','str9_1024x1024.txt']
#prob = ['dstr1_512x512.txt', 'dstr5_512x512.txt', 'dstr9_512x512.txt', 'str2_512x512.txt', 'str3_512x512.txt', 'str4_512x512.txt', 'str6_512x512.txt', 'str7_512x512.txt', 'str8_512x512.txt', 'str10_512x512.txt']
#prob =['str5_256x256.txt','str5_256x512.txt','str5_256x1024.txt','str5_256x2048.txt','str5_256x4096.txt']
#tr5_256x256.txt','str5_512x256.txt','str5_1024x256.txt','str5_2048x256.txt','str5_4096x256.txt']
#prob = ['str5_256x256.txt','str5_512x256.txt','str5_1024x256.txt','str5_2048x256.txt','str5_4096x256.txt']
#prob = 
# warning, don't remove 0, main.py doesn't rectify
#do not add 0 we are not running this for serial
procs = [0,2,4,8,12,16]


##############################################################
# prob = ['2x2.txt', '3x2.txt']
# prob = ['str1_32x32.txt', 'str1_32x64.txt', 'str1_32x128.txt', 'str1_32x256.txt', 'str1_32x1024.txt', 'str1_64x32.txt', 'str1_64x64.txt', 'str1_64x128.txt', 'str1_64x256.txt', 'str1_128x32.txt', 'str1_128x64.txt', 'str1_128x128.txt', 'str1_256x32.txt', 'str1_256x64.txt']

#netlib
#prob=['adlittle_56x138.txt','afiro_27x51.txt','agg_488x615.txt','agg2_516x758.txt','bandm_305x427.txt','pilot4_410x1123.txt','ken07_2426x3602.txt','scsd6_147x1350.txt']

#varying variables
prob = ['str5_512x256.txt', 'str5_512x512.txt', 'str5_512x1024.txt', 'str5_512x2048.txt', 'str5_512x4096.txt']

#varying equations
#prob=['str1_256x256.txt', 'str1_512x512.txt', 'str1_1024x1024.txt', 'str1_2048x2048.txt', 'str5_256x256.txt', 'str5_512x512.txt', 'str5_1024x1024.txt', 'str5_2048x2048.txt','str9_256x256.txt', 'str9_512x512.txt', 'str9_1024x1024.txt', 'str9_2048x2048.txt']
