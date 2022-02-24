import subprocess
import time


def run_cmd(cmd):
    print(" ".join(cmd))
    try:
        proc = subprocess.run(
            cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, check=True)
        return proc.stdout
    except subprocess.CalledProcessError:
        print("Error:")
        print(" ".join(cmd))
        exit(-1)


sizes = [128, 256, 512, 1024]
exes = ["base", "sse", "avx2", "avx512"]
infos = [
    "",
    "uchar",
    "float",
    "",
    "uchar",
    "float",
    "",
    "uchar 1",
    "uchar 3",
    "float 1",
    "float 3",
    "",
    "uchar",
    "uchar masked",
    "",
    "uchar",
    "uchar masked",
    "float",
    "float masked",
    "",
    "uchar",
    "uchar masked",
    "float",
    "float masked",
    "",
    "float",
    "",
    "uchar 1",
    "uchar 3",
    "float 1",
    "float 3",
    "",
    "uchar 1",
    "uchar 3",
    "",
    "uchar row 1",
    "uchar row 3",
    "uchar col",
    "float row 1",
    "float row 3",
    "float col",
    "",
    "row ksize 3",
    "row ksize 5",
    "col ksize 3",
    "col ksize 5",
    "col ksize 3 symm 1",
    "col ksize 3 symm 0",
    "col ksize 5 symm 1",
    "col ksize 5 symm 0"
]


def run():
    for size in sizes:
        run_cmd(["make", "WIDTH={}".format(size), "HEIGHT={}".format(size)])
        for exe in exes:
            output = run_cmd(["./opencv-" + exe])
            with open("./result/" + str(size) + "-" + exe + ".txt", "w") as f:
                f.write(output)


def gen():
    for size in sizes:
        outputs = []
        for exe in exes:
            with open("./result/" + str(size) + "-" + exe + ".txt", "r") as f:
                outputs.append(f.readlines()[1:])
        with open("./result/" + str(size) + ".txt", "w") as f:
            f.write("\t" + "\t".join(exes))
            f.write("\t" + "\t".join(exes[1:]) + "\n")
            for i in range(len(outputs[0])):
                if ":" in outputs[0][i]:
                    f.write(outputs[0][i][0:-2])
                else:
                    f.write(infos[i] + "\t")
                    times = []
                    for j in range(len(outputs)):
                        line = outputs[j][i]
                        times.append(line.split()[0])
                    base_time = float(times[0])
                    for i in range(1, len(times)):
                        times.append("{:.4f}".format(
                            base_time / float(times[i])))
                    f.write("\t".join(times))
                f.write("\n")
    pass


if __name__ == "__main__":
    run()
    gen()
