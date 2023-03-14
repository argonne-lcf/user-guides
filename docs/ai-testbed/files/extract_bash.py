import os
import re


def extract_bash_02(f: str) -> str:
    """
    Extracts all bash code blocks from the specified input file,
    concatenates them, and writes the concatenated code blocks to a
    new file with the same name as the input file but with a `.sh`
    extension. Returns the name of the output file.

    Args:
        f (str): The name of the input file to extract code blocks from.

    Returns:
        str: The name of the output file that the extracted code blocks were written to.
    """
    pattern = r'bash\n(.*?)\n'

    with open(f, "r") as file:
        lines = file.readlines()
        string = "\n".join(lines)
        matches = re.findall(pattern, string, re.DOTALL)

    for match in matches:
        print(match)

    if not matches:
        return ""

    output_filename = os.path.splitext(f)[0] + ".sh"
    with open(output_filename, "w") as output_file:
        output_file.write("\n".join(matches))

    return output_filename

def extract_bash(f: str) -> str:
    """
    Extracts all bash code blocks from the specified input file,
    concatenates them, and writes the concatenated code blocks to a
    new file with the same name as the input file but with a `.sh`
    extension. Returns the name of the output file.

    Args:
        f (str): The name of the input file to extract code blocks from.

    Returns:
        str: The name of the output file that the extracted code blocks were written to.
    """
    with open(f, "r") as file:
        lines = file.readlines()
        start, end = -1, -1
        bash_blocks = []
        for i, line in enumerate(lines):
            if line.startswith("```bash"):
                if start == -1:
                    start = i
            if line.startswith("```\n") and start != -1:
                end = i
                bash_blocks.append("".join(lines[start+1:end]))
                start = -1
                end = -1
        if not bash_blocks:
            return ""

        output_filename = os.path.splitext(f)[0] + ".sh"
        with open(output_filename, "w") as output_file:
            output_file.write("\n".join(bash_blocks))

        return output_filename


def main() -> None:
    """
    Extracts all bash code blocks from all markdown files (`*.md`) in the
    current working directory and writes them to new files with the same
    name as the input files but with a `.sh` extension.
    """
    md_files: list[str] = [f for f in os.listdir() if f.endswith(".md")]
    for f in md_files:
        print(f"Extracting bash code blocks from {f}...")
        output_file: str = extract_bash(f)
        print(f"Extracted code blocks saved to {output_file}")


if __name__ == "__main__":
    main()
