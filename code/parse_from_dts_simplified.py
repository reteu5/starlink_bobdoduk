import os

# 현재 파이썬 스크립트의 위치를 가져옵니다.
script_dir = os.path.dirname(os.path.abspath(__file__))

# 파일 경로를 지정합니다.
input_base_path = os.path.join(script_dir, r"../src/dts_original/")
output_base_path = os.path.join(script_dir, r"../src/dts_processed/")

def list_local_files(directory_path) :
    # 디렉토리 내의 파일들을 리스트로 반환합니다.
    return [os.path.join(directory_path, file_name) for file_name in os.listdir(directory_path)]

local_original_files = list_local_files(input_base_path)

for dts_file in local_original_files:
    file_name = os.path.basename(dts_file)
    file_name_without_ext = os.path.splitext(file_name)[0]
    input_file_path = input_base_path + file_name
    output_file_path = output_base_path + file_name_without_ext + "_output.txt"

    # 입력 파일을 open, readlines로 읽어옵니다.
    with open(input_file_path, "r", encoding="utf-8") as infile:
        lines = infile.readlines()
        # print(lines)

    # 출력 파일에 `{` 다음에 줄바꿈이 오는 줄들 ('{\n'으로 끝나는 줄)만을 이어붙입니다.
    with open(output_file_path, "w", encoding="utf-8") as outfile:
        for line in lines:
            if (line.strip().endswith("{") | line.strip().endswith("}")):
                print(line)
                outfile.write(line)

    print(f"[-] Work Done! Checkout {output_file_path} file.")
    