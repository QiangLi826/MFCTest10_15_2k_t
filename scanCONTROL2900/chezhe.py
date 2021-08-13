import math
import numpy as np
import matplotlib.pyplot as plot

import sympy

def paint():
    ##17个数据点，并进行去横坡处理
    f = [0, 5, -5, -15, -15, -12.47, -2.48, 7.5, 10, 8, 0, -7.96, -10, -10, -2.48, 5, 0]
    y= f[:]
    zero = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    B = []

    n = 17
    x = [0, 200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400, 2600, 2800, 3000, 3200]


    ang = math.atan((y[16] - y[0]) / 3200) # %由测点1与测点17计算坐标转动角度angle
    A = [x[:], y[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:]]
    for i in range(n): # 对测点坐标系进行转换，将测点1设为原点，测点1与测点17连线设为直线
        if ang >= 0:
            x[i] = A[0][i]*math.cos(ang)-(A[1][i]-A[1][0])*math.sin(ang)
            y[i]=-(A[0][i]*math.sin(ang)-(A[1][i]-A[1][0])*math.cos(ang))
        else:
            x[i] = A[0][0] * math.cos(ang) + (A[1][0] - A[1][0]) * math.sin(ang)
            y[i] = -(-A[0][i] * math.sin(ang) + (A[1][i] + A[1][0]) * math.cos(ang))
    # 显示车辙采样点数据
    plot.plot(x, y, 'o')
    #plot.show()

    #  绘制车辙曲线图形
    interval = 0.25
    dy1 = 0
    dyn = 0
    h = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    h[n - 2] = x[n - 1] - x[n - 2]

    v = []
    u = []
    for i in range(0, n - 2):
        h[i] = x[i + 1] - x[i]
        v.append(h[i + 1] / (h[i + 1] + h[i]))
        u.append(1 - v[i])

    g = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    g[0] = 3 * (y[1] - y[0]) / h[0] - h[0] / 2 * dy1
    g[n - 1] = 3 * (y[n - 1] - y[n - 2]) / h[n - 2] + h[n - 2] / 2 * dyn

    for i in range(1, n - 1):
        g[i] = 3 * (u[i - 1] * (y[i + 1] - y[i]) / h[i] + v[i - 1] * (y[i] - y[i - 1]) / h[i - 1])

    for i in range(1, n - 1):
        A[i][i - 1] = v[i - 1]
        A[i][i + 1] = u[i - 1]

    A[n - 1][n - 2] = 1
    A[0][1] = 1

    A = A + 2 * np.eye(n)

    # 追赶法求解三对角方程，得到矩阵M
    L = np.eye(n)
    U = np.zeros([n, n])

    for i in range(0, n - 1):
        U[i][i + 1] = A[i][i + 1]

    U[0][0] = A[0][0]

    for i in range(1, n):
        L[i][i - 1] = A[i][i - 1] / U[i - 1][i - 1]
        U[i][i] = A[i][i] - L[i][i - 1] * A[i - 1][i]

    Y = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    Y[0] = g[0]
    for i in range(1, n):
        Y[i] = g[i] - L[i][i - 1] * Y[i - 1]

    M = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    M[n - 1] = Y[n - 1] / U[n - 1][n - 1]

    for i in range(n - 2, -1, -1):
        M[i] = (Y[i] - A[i][i + 1] * M[i + 1]) / U[i][i]

    X = sympy.symbols('x')
    b = 0
    s = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    h = np.array(h)
    x = np.array(x)
    y = np.array(y)
    # for k in range(n-1):
    #     s[k] = (h[k]+2*(X-x[k]))/h[k]**3*(X-x[k+1])**2*y[k] \
    #     + (h[k]-2*(X-x[k+1]))/h[k]**3*(X-x[k])**2*y[k+1] \
    #     +(X-x[k])*(X-x[k+1])**2/h[k]**2*M[k] \
    #     +(X-x[k+1])*(X - x[k])** 2 / h[k]**2 * M[k + 1]

    # 画三次样条插值函数图像
    for i in range(n - 1):
        X = np.arange(x[i], x[i + 1], 0.25)
        st = (h[i] + 2 * (X - x[i])) / (h[i] ** 3) * (X - x[i + 1]) ** 2 * y[i] \
             + (h[i] - 2 * (X - x[i + 1])) / (h[i] ** 3) * (X - x[i]) ** 2 * y[i + 1] \
             + (X - x[i]) * (X - x[i + 1]) ** 2 / h[i] ** 2 * M[i] + (X - x[i + 1]) * \
             (X - x[i]) ** 2 / h[i] ** 2 * M[i + 1]
        plot.plot(x, y, 'o', X, st, 'b.', 'LineWidth', 2)

    plot.plot([0, 3200], [0, 0], '-k', 'LineWidth', 2)
    # plot.grid(True)
    #plot.show(block=False)
    plot.show()

#判断车辙曲线图形类型
def get_kind(f):
    #f = [0, 5, -5, -15, -15, -12.47, -2.48, 7.5, 10, 8, 0, -7.96, -10, -10, -2.48, 5, 0]
    y = f[:]
    zero = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    x = [0, 200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400, 2600, 2800, 3000, 3200]

    ang = math.atan((y[16] - y[0]) / 3200)  # %由测点1与测点17计算坐标转动角度angle
    A = [x[:], y[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:], zero[:],
         zero[:], zero[:], zero[:], zero[:]]

    h = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    h[16] = abs(y[16] - y[0])
    h[0] = 0
    D = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    D[16] = 3200
    for j in range(17):
        #先对数据进行防抖动处理，对测点坐标系进行转换，将测点1设为原点，测点1与测点17连线设为直线
        if ang >= 0:
            D[j] = j * 200             #数据防抖动处理
            h[j] = D[j] * h[16] / D[16]
            y[j] = y[j] - h[j]

            x[j] = A[0][j]*math.cos(ang)-(A[1][j]-A[1][0])*math.sin(ang)
            y[j]=-(A[0][j]*math.sin(ang)-(A[1][j]-A[1][0])*math.cos(ang))
        else:
            D[j] = j * 200  # 数据防抖动处理
            h[j] = D[j] * h[16] / D[16]
            y[j] = y[j] + h[j]

            x[j] = A[0][j] * math.cos(ang) + (A[1][j] - A[1][0]) * math.sin(ang)
            y[j] = -(-A[0][j] * math.sin(ang) + (A[1][j] + A[1][0]) * math.cos(ang))

    yl = [y[0], y[1], y[2], y[3], y[4], y[5], y[6], y[7]]  # 以测点9为中心，将测点分为左、右两类
    yr = [y[9], y[10], y[11], y[12], y[13], y[14], y[15], y[16]]

    stdl = np.std(yl, 0) # 计算左测点、右侧点标准差
    stdr = np.std(yr, 0)
    P = np.polyfit(x, y, 6); # 根据转换后的测点坐标进行6次多项式拟合
    #Y = str(np.poly2str(P, 'X')) # 定义车辙函数自变量为'x'.
    #plot.plot(x, y, 'o')
    #plot.show()
    R = np.min(np.corrcoef(y, np.polyval(P, x)))
    dP = np.polyder(P) # 求车辙函数一阶导数
    xi = np.arange(x[0], x[16], 1) # 计算测点横坐标
    yi = np.polyval(P, xi)  #计算测点函数值
    r = np.roots(dP)       #求车辙函数极值点横坐标
    r = r[np.where((r >= 0) & (r <= x[16]) & (abs(np.imag(r)) <= 2.2204e-16))] # 规定极值点横坐标范围为(0, x17)   且均为实数

    m = len(r) # 计算极值点个数
    v = np.polyval(P, r) #计算极值点函数值
    n = v[np.where(v < 0)] #计算函数值小于0的极值点个数

    kind = 0 # 车辙类型判断参数，A - 1，B - 2，...，G - 7
    if m == 1: # 极值点个数为1的情况
        kind = 7
    elif  m == 2:
        kind = 7
    elif m == 3:
        if np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 \
                and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[2])) > 5:
            kind = 1
        elif np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and abs(
                np.polyval(P, r[0])) > 5 and abs(
                np.polyval(P, r(2))) > 5:
            kind = 6
        elif (np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and abs(
                np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[2])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and abs(
            np.polyval(P, r[0])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and abs(
            np.polyval(P, r[2])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0):
            kind = 7
        else:
            kind = 8;

    elif  m == 4:
        if (np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and abs(np.polyval(P, r[3])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[0])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[3])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[0])) <= 5):
            kind = 1

        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[0])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and abs(np.polyval(P, r[3])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[0])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[3])) <= 5):
            kind = 6
        elif (np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[3])) > 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[3])) > 5):
            kind = 3
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) > 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[3])) > 5):
            kind = 5
        else:
            kind = 8

    elif m == 5:
        if np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) > 5:
            kind = 2
        elif np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) > 5:
            kind = 4
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5):
            kind = 3
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5):
            kind = 3
        elif (np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5):
            kind = 3
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(
                np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(
                np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(
                np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) < 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) <= 5):
            kind = 1
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5):
            kind = 5
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) < 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5):
            kind = 5
        elif (np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) > 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) < 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) > 5 and abs(np.polyval(P, r[4])) <= 5):
            kind = 5
        elif (np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(
                np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) < 0 and abs(np.polyval(P, r[0])) <= 5 and abs(
                np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) < 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(
                np.polyval(P, r[4])) <= 5) or (
                np.polyval(P, r[0]) > 0 and np.polyval(P, r[1]) > 0 and np.polyval(P, r[2]) > 0 and np.polyval(P, r[
            3]) > 0 and np.polyval(P, r[4]) > 0 and abs(np.polyval(P, r[0])) <= 5 and abs(np.polyval(P, r[4])) <= 5):
            kind = 6
        else:
            kind = 8
    else:
        kind = 8

    return kind

def get_RD(f):
    #f = [0, 5, -5, -15, -15, -12.47, -2.48, 7.5, 10, 8, 0, -7.96, -10, -10, -2.48, 5, 0]
    y = f[:]
    zero = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    x = [0, 200, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2200, 2400, 2600, 2800, 3000, 3200]
    #将车辙曲线分为五段
    A = [y[0], y[1], y[2], y[3]]
    B = [y[3], y[4], y[5], y[6]]
    C = [y[7], y[8], y[9]]
    D = [y[10], y[11], y[12], y[13]]
    E = [y[13], y[14], y[15], y[16]]

    p1 = np.argmax(A)
    m1 = A[p1]

    p2 = np.argmin(B)
    m2 = B[p2]

    p3 = np.argmax(C)
    m3 = C[p3]

    p4 = np.argmin(D)
    m4 = D[p4]

    p5 = np.argmax(E)
    m5 = E[p5]

    # 求最大最小值对应的横坐标
    n1 = x[p1]
    n2 = x[3 + p2]
    n3 = x[7 + p3]
    n4 = x[10 + p4]
    n5 = x[13 + p5]
    # print(n1,n2,n3,n4)
    if m3 <= 0:
        xa = [n1, n5]
        ya = [m1, m5]
        Pa = np.polyfit(xa, ya, 1)
        dal = abs(m2) + abs(np.polyval(Pa, n2))
        dar = abs(m4) + abs(np.polyval(Pa, n4))
        RD1 = dal
        RD2 = dar
    else:
        xal = [n1, n3]
        yal = [m1, m3]
        xar = [n3, n5]
        yar = [m3, m5]
        Pal = np.polyfit(xal, yal, 1)
        Par = np.polyfit(xar, yar, 1)
        dal = abs(m2) + abs(np.polyval(Pal, n2))
        dar = abs(m4) + abs(np.polyval(Par, n4))
        RD1 = dal
        RD2 = dar

    print(RD1, RD2)
    kind = get_kind(f)
    return kind, RD1, RD2
if __name__ == '__main__':
    f = [0, 5, -5, -15, -15, -12.47, -2.48, 7.5, 10, 8, 0, -7.96, -10, -10, -2.48, 5, 0]
    get_RD(f)




