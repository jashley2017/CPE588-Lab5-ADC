from serial import Serial
import matplotlib.pyplot as plt


BAUDRATE = 115200
PORT = 5

if __name__=='__main__':
    wave_bytes = int(input("How many points would you like to sample: "))*4 # up to 4 chars/sample
    tiva_port = Serial(f"/dev/ttyS{PORT}", baudrate=115200)
    result = tiva_port.read(size=wave_bytes).decode("utf-8")
    results = result.split("\r\n")
    voltages = [int(res)/4095 for res in results]
    xs = [x*5 for x in range(len(voltages))]
    plt.plot(xs, voltages)
    plt.xlabel("Time (ms)")
    plt.ylabel("Wave (V)")
    plt.show()
