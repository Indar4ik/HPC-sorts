import re
from pathlib import Path

def parse_sortnet_html(html_path: str) -> dict[int, tuple[int, list[list[tuple[int, int]]]]]:
    """
    Парсит HTML файл с сортировочными сетями.
    Возвращает словарь: N -> (min_CEs, layers)
    где layers — список слоёв, каждый слой — список пар (i, j).
    """
    html = Path(html_path).read_text(encoding="utf-8")

    # Каждый блок начинается с <tr id="N...">
    # Разбиваем HTML по началу каждой записи
    block_pattern = re.compile(
        r'<tr id="N(\d+)L(\d+)D\d+">'   # группы: N, CEs
        r'.*?'                            # всё между
        r'<p class="mono">(.*?)</p>',     # содержимое моно-блока
        re.DOTALL
    )

    best: dict[int, tuple[int, list[list[tuple[int, int]]]]] = {}

    for match in block_pattern.finditer(html):
        n = int(match.group(1))
        ces = int(match.group(2))
        mono_content = match.group(3)

        # Каждая строка вида [(i,j),(k,l),...]<br> — один слой
        layer_pattern = re.compile(r'\[([^\]]*)\]')
        pair_pattern = re.compile(r'\((\d+),(\d+)\)')

        layers: list[list[tuple[int, int]]] = []
        for layer_match in layer_pattern.finditer(mono_content):
            pairs = pair_pattern.findall(layer_match.group(1))
            if pairs:
                layers.append([(int(a), int(b)) for a, b in pairs])

        if not layers:
            continue

        # Оставляем только сеть с минимальным CEs для данного N
        if n not in best or ces < best[n][0]:
            best[n] = (ces, layers)

    return best


def generate_cpp(networks: dict[int, tuple[int, list[list[tuple[int, int]]]]]) -> str:
    """
    Генерирует C++ код для всех сортировочных сетей.
    """
    lines: list[str] = []
    lines.append("// Auto-generated sorting networks")
    lines.append("// Source: https://bertdobbelaere.github.io/sorting_networks.html")
    lines.append("")
    lines.append("inline constexpr void cmp(int& a, int& b) noexcept {")
    lines.append("    if (a > b) { int t = a; a = b; b = t; }")
    lines.append("}")
    lines.append("")

    for n in sorted(networks):
        ces, layers = networks[n]
        lines.append(f"// {n} inputs, {ces} CEs")
        lines.append(f"inline constexpr void sort{n}(int* a) noexcept {{")

        for layer in layers:
            cmps = "; ".join(f"cmp(a[{i}], a[{j}])" for i, j in layer)
            lines.append(f"    {cmps};")

        lines.append("}")
        lines.append("")

    return "\n".join(lines)


def main():
    html_path = "/mnt/user-data/uploads/sortnet_numbers.html"
    output_path = "/mnt/user-data/outputs/sorting_networks.cpp"

    print(f"Парсинг {html_path}...")
    networks = parse_sortnet_html(html_path)

    print(f"Найдено сетей: {len(networks)}")
    for n in sorted(networks):
        ces, layers = networks[n]
        total = sum(len(l) for l in layers)
        assert total == ces, f"N={n}: заявлено {ces} CEs, насчитано {total}"
        print(f"  N={n:2d}: {ces:3d} CEs, {len(layers)} слоёв")

    cpp = generate_cpp(networks)
    Path(output_path).write_text(cpp, encoding="utf-8")
    print(f"\nГотово! C++ код записан в {output_path}")


if __name__ == "__main__":
    main()
