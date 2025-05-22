import multiprocessing
from math import sqrt
from copy import deepcopy

def dot_product(v1, v2):
    """Скалярное произведение двух векторов."""
    return sum(x * y for x, y in zip(v1, v2))

def vector_norm(v):
    """Евклидова норма вектора."""
    return sqrt(dot_product(v, v))

def subtract_vectors(v1, v2):
    """Вычитание векторов."""
    return [x - y for x, y in zip(v1, v2)]

def scalar_multiply(v, scalar):
    """Умножение вектора на скаляр."""
    return [x * scalar for x in v]

def modified_gram_schmidt(A):
    """Модифицированный процесс Грама-Шмидта для QR-разложения."""
    n = len(A)
    Q = [[0.0] * n for _ in range(n)]
    R = [[0.0] * n for _ in range(n)]
    
    for j in range(n):
        v = [A[i][j] for i in range(n)]  # j-й столбец A
        for i in range(j):
            R[i][j] = dot_product([Q[k][i] for k in range(n)], v)
            v = subtract_vectors(v, scalar_multiply([Q[k][i] for k in range(n)], R[i][j]))
        norm = vector_norm(v)
        R[j][j] = norm
        if norm > 1e-10:  # Проверка на нуль, чтобы избежать деления
            for i in range(n):
                Q[i][j] = v[i] / norm
    return Q, R

def matrix_vector_multiply(M, v):
    """Умножение матрицы на вектор."""
    return [dot_product(row, v) for row in M]

def solve_upper_triangular(R, b):
    """Решение верхнетреугольной системы Rx = b методом обратной подстановки."""
    n = len(R)
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        if abs(R[i][i]) < 1e-10:
            raise ValueError("Матрица вырождена или плохо обусловлена")
        x[i] = b[i]
        for j in range(i + 1, n):
            x[i] -= R[i][j] * x[j]
        x[i] /= R[i][i]
    return x

def solve_system(A_k, b_k):
    """Решение системы A_k x_k = b_k для размера k."""
    k = len(A_k)
    # QR-разложение
    Q, R = modified_gram_schmidt(A_k)
    # Вычисление Q^T b
    Q_T_b = matrix_vector_multiply([[Q[i][j] for j in range(k)] for i in range(k)], b_k)
    # Решение R x = Q^T b
    x = solve_upper_triangular(R, Q_T_b)
    return x

def prepare_subsystem(A, b, k):
    """Подготовка подматрицы A_k и вектора b_k размера k."""
    A_k = [[A[i][j] for j in range(k)] for i in range(k)]
    b_k = b[:k]
    return A_k, b_k

def main(A, b, max_k):
    """Основная функция для параллельного решения вложенных систем."""
    # Создаем пул процессов
    pool = multiprocessing.Pool()
    
    # Подготавливаем аргументы для каждой системы
    tasks = [(deepcopy(prepare_subsystem(A, b, k))) for k in range(1, max_k + 1)]
    
    # Запускаем параллельное решение
    results = pool.starmap(solve_system, tasks)
    
    # Закрываем пул
    pool.close()
    pool.join()
    
    return results

# Пример использования
if __name__ == "__main__":
    # Пример матрицы A (4x4) и вектора b
    A = [
        [4.0, 1.0, 0.0, 0.0],
        [1.0, 3.0, 1.0, 0.0],
        [0.0, 1.0, 2.0, 1.0],
        [0.0, 0.0, 1.0, 1.0]
    ]
    b = [1.0, 2.0, 3.0, 4.0]
    max_k = 4  # Максимальный размер системы
    
    # Решение всех вложенных систем
    solutions = main(A, b, max_k)
    
    # Вывод результатов
    for k, x in enumerate(solutions, 1):
        print(f"Решение для k={k}: {x}")