#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]);
int congruence(int a, int b, int c, int *error);
int i4_gcd(int i, int j);
int i4_max(int i1, int i2);
int i4_min(int i1, int i2);
int i4_sign(int i);
void lcrg_anbn(int a, int b, int c, int n, int *an, int *bn);
int lcrg_evaluate(int a, int b, int c, int x);
int power_mod(int a, int n, int m);
void timestamp();

int main(int argc, char *argv[])
{
  int a;
  int an;
  int b;
  int bn;
  int c;
  int error;
  int id;
  int j;
  int k;
  int k_hi;
  int p;
  int u;
  int v;
  double start_time, end_time, com_start_time, com_end_time;
  double total_time = 0.0;
  double total_com_time = 0.0;
  double total_time_show;
  int total_number = 2000000000;

  /*
    Khởi tạo MPI
  */
  error = MPI_Init(&argc, &argv);

  if (error != 0)
  {
    printf("\n");
    printf("RANDOM_MPI - Fatal error.\n");
    printf("MPI_Init returned nonzero ERROR.\n");
    exit(1);
  }

  /*
    Lấy số lượng processors.
  */
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  /*
    Lấy hạng của processor.
  */

  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  start_time = MPI_Wtime();

  /*
    In message ra màn hình của máy master.
  */
  if (id == 0)
  {
    timestamp();
    printf("\n");
    printf("RANDOM_MPI - Master process:\n");
    printf("The number of processors is P = %d\n", p);
    printf("\n");
    printf("This program shows how a stream of random numbers\n");
    printf("can be computed 'in parallel' in an MPI program.\n");
    printf("\n");
    printf("We assume we are using a linear congruential\n");
    printf("random number generator or LCRG, which takes\n");
    printf("an integer input and returns a new integer output:\n");
    printf("\n");
    printf("\t\tU = ( A * V + B ) mod C\n");
    printf("\n");
    printf("We assume that we want the MPI program to produce\n");
    printf("the same sequence of random values as a sequential\n");
    printf("program would - but we want each processor to compute\n");
    printf("one part of that sequence.\n");
    printf("\n");
    printf("We do this by computing a new LCRG which can compute\n");
    printf("every P'th entry of the original one.\n");
    printf("\n");
    printf("Our LCRG works with integers, but it is easy to\n");
    printf("turn each integer into a real number between [0,1].\n");
  }

  /*
    A, B và C định nghĩa linear congruential random number generator.
  */
  a = 16807;
  b = 0;
  c = 2147483647;

  if (id == 0)
  {
    printf("\n");
    printf("LCRG parameters:\n");
    printf("\n");
    printf("A  = %d\n", a);
    printf("B  = %d\n", b);
    printf("C  = %d\n", c);
  }

  int number_each_process = total_number / p;
  k_hi = p * number_each_process;

  /*
    Bắt đầu đo thời gian.
  */
  // if (id == 0)
  // {
  //   start_time = MPI_Wtime();
  // }

  /*
    Processor 0 sinh ra 10 * P giá trị random.
  */
  // if (id == 0)
  // {
  //   printf("\n");
  //   printf("  Let processor 0 generate the entire random number sequence.\n");
  //   printf("\n");
  //   printf("     K    ID         Input        Output\n");
  //   printf("\n");

  //   k = 0;
  //   v = 12345;
  //   printf("  %4d  %4d                %12d\n", k, id, v);

  //   for (k = 1; k <= k_hi; k++)
  //   {
  //     u = v;
  //     v = lcrg_evaluate(a, b, c, u);
  //     printf("  %4d  %4d  %12d  %12d\n", k, id, u, v);
  //   }
  // }

  /*
    Processor P tham gia bằng cách tính toán phần thứ P của chuỗi.
  */
  lcrg_anbn(a, b, c, p, &an, &bn);

  if (id == 0)
  {
    printf("\n");
    printf("  LCRG parameters for P processors:\n");
    printf("\n");
    printf("  AN = %d\n", an);
    printf("  BN = %d\n", bn);
    printf("  C  = %d\n", c);
    printf("\n");
    printf("  Have ALL the processors participate in computing\n");
    printf("  the same random number sequence.\n");
    printf("\n");
    printf("     K    ID         Input        Output\n");
    printf("\n");
  }
  /*
    Lấy các giá trị thứ ID trong chuỗi.
  */
  v = 12345;
  printf("Id: %d\n", id);
  for (j = 1; j <= id; j++)
  {

    // start_time = MPI_Wtime();

    u = v;
    v = lcrg_evaluate(a, b, c, u);

    // end_time = MPI_Wtime();
    // total_time += end_time - start_time;
  }
  // start_time = MPI_Wtime();
  k = id;
  printf("v: %d\n", v);

  printf("  %4d  %4d                %12d --\n", k, id, v);
  // end_time = MPI_Wtime();
  // total_time += end_time - start_time;
  
  /*
    Sử dụng LCRG để tính toán các giá trị với chỉ số ID, ID + P, ID + 2P, ...
  */
  for (k = id + p; k <= k_hi; k = k + p)
  {

    // start_time = MPI_Wtime();

    u = v;
    v = lcrg_evaluate(an, bn, c, u);
    // printf("  %4d  %4d  %12d  %12d ---\n", k, id, u, v);

    // end_time = MPI_Wtime();
    // total_time += end_time - start_time;
  }

  /*
    Đo thời gian truyền thông
  */

  // Tiến hành truyền thông: Tiến trình 0 gửi giá trị cuối cùng của nó đến tất cả các tiến trình khác.
  if (id == 0)
  {
    com_start_time = MPI_Wtime();
    for (int dest = 1; dest < p; dest++)
    {
      MPI_Send(&v, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
    com_end_time = MPI_Wtime();
    total_com_time = total_com_time + com_end_time - com_start_time;
  }
  else
  {
    com_start_time = MPI_Wtime();
    int received_value;
    MPI_Recv(&received_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Processor %d received value %d from processor 0\n", id, received_value);
    com_end_time = MPI_Wtime();
    total_com_time = total_com_time + com_end_time - com_start_time;
  }

  /*
    End timing
  */
  // if (id == 0)
  // {
    end_time = MPI_Wtime();
    total_time = end_time - start_time;
  // }

  MPI_Reduce(&total_time, &total_time_show, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  /*
    In thời gian thực thi tổng thể tại processor 0.
  */
  // if (id == 0)
  // {
  //   printf("Total execution time (T_w_com): %f seconds.\n", total_time);
  //   printf("Execution time without communication (T_wo_com): %f seconds.\n", total_time - total_com_time);
  //   printf("Each processor has %d numbers\n", number_each_process);
  //   printf("Total number of random numbers: %d\n", total_number);
  //   printf("Total of processors: %d\n", p);
  // }

  /*
    Terminate MPI.
  */
  MPI_Finalize();

  if (id == 0)
  {
    printf("Total communication time: %f\n", total_com_time);
    printf("Total execution time (T_w_com): %f seconds.\n", total_time);
    printf("Execution time without communication (T_wo_com): %f seconds.\n", total_time - total_com_time);
    printf("Each processor has %d numbers\n", number_each_process);
    printf("Total number of random numbers: %d\n", total_number);
    printf("Total of processors: %d\n", p);
  }
  if (id == 0)
  {
    printf("\n");
    printf("RANDOM_MPI:\n");
    printf("  Normal end of execution.\n");
    printf("\n");
    timestamp();
  }
  return 0;
}

int congruence(int a, int b, int c, int *error)

/******************************************************************************/
/*
  Purpose:

    CONGRUENCE sử dụng để giải phương trình đồng dư A * X = C ( mod B ).

  Discussion:

    A, B và C là các số nguyên cho trước.
    Phương trình có thể giải được nếu và chỉ nếu ước chung lớn nhất của A và B cũng chia hết cho C.

  Parameters:

    Input, int A, B, C, hệ số của phương trình Diophantine.

    Output, int *ERROR, error flag, có giá trị là 1 nếu có lỗi xảy ra

    Output, int CONGRUENCE, nghiệm của phương trình Diophantine.
    X sẽ nằm trong khoảng giữa 0 và B-1.
*/
{
#define N_MAX 100

  int a_copy;
  int a_mag;
  int a_sign;
  int b_copy;
  int b_mag;
  int c_copy;
  int g;
  int k;
  int n;
  int q[N_MAX];
  int swap;
  int x;
  int y;
  int z;
  /*
    Tham số mặc định cho đầu ra.
  */
  *error = 0;
  x = 0;
  y = 0;
  /*
    Các trường hợp đặc biệt.
  */
  if (a == 0 && b == 0 && c == 0)
  {
    x = 0;
    return x;
  }
  else if (a == 0 && b == 0 && c != 0)
  {
    *error = 1;
    x = 0;
    return x;
  }
  else if (a == 0 && b != 0 && c == 0)
  {
    x = 0;
    return x;
  }
  else if (a == 0 && b != 0 && c != 0)
  {
    x = 0;
    if ((c % b) != 0)
    {
      *error = 2;
    }
    return x;
  }
  else if (a != 0 && b == 0 && c == 0)
  {
    x = 0;
    return x;
  }
  else if (a != 0 && b == 0 && c != 0)
  {
    x = c / a;
    if ((c % a) != 0)
    {
      *error = 3;
      return x;
    }
    return x;
  }
  else if (a != 0 && b != 0 && c == 0)
  {
    /*  g = i4_gcd ( a, b ); */
    /*  x = b / g; */
    x = 0;
    return x;
  }
  /*
    Giải quyết trường hợp tổng quát: A, B và C khác 0.

    Step 1: Tính giá trị GCD của A và B, đồng thời chia hết cho C.
  */
  g = i4_gcd(a, b);

  if ((c % g) != 0)
  {
    *error = 4;
    return x;
  }

  a_copy = a / g;
  b_copy = b / g;
  c_copy = c / g;
  // a/g * x = c/g mod b/g
  // gcd(a, b) = g
  // gcd(a/g, b/g) = 1
  // a/g * x + b /g * y = c / g
  // a_mag * x + b_mag * y = c_copy
  // a_mag * x + b_mag * y = 1
  /*
    Step 2: Tính độ lớn của A, B và dấu của A
  */
  a_mag = abs(a_copy);
  a_sign = i4_sign(a_copy);
  b_mag = abs(b_copy);
  /*
    Trường hợp đặc biệt khác, A_MAG = 1 hoặc B_MAG = 1.
  */
  if (a_mag == 1)
  {
    x = a_sign * c_copy;
    return x;
  }
  else if (b_mag == 1)
  {
    x = 0;
    return x;
  }
  /*
    Step 3: Tạo chuỗi số dư Euclide.
  */
  if (b_mag <= a_mag)
  {
    swap = 0;
    q[0] = a_mag;
    q[1] = b_mag;
  }
  else
  {
    swap = 1;
    q[0] = b_mag;
    q[1] = a_mag;
  }

  n = 3;

  for (;;)
  {
    q[n - 1] = (q[n - 3] % q[n - 2]);

    if (q[n - 1] == 1)
    {
      break;
    }

    n = n + 1;

    if (N_MAX < n)
    {
      *error = 1;
      printf("\n");
      printf("CONGRUENCE - Fatal error!\n");
      printf("  Exceeded number of iterations.\n");
      exit(1);
    }
  }
  /*
    Step 4: Quay lại giải X * A_MAG + Y * B_MAG = 1.
  */
  y = 0;
  for (k = n; k >= 2; k--)
  {
    x = y;
    y = (1 - x * q[k - 2]) / q[k - 1];
  }
  /*
    Step 5: Undo swapping (Nếu có swap).
  */
  if (swap == 1)
  {
    z = x;
    x = y;
    y = z;
  }
  /*
    Step 6: Áp dụng giá  trị dấu vào X và Y ==> X * A + Y * B = 1.
  */
  x = x * a_sign;
  /*
    Step 7: Nhân với C ==> X * A + Y * B = C. AX/C = 1 mod B.
  */
  x = x * c_copy;
  /*
    Step 8: Lấy giá trị trong khoảng 0 <= X < B.
  */
  x = x % b;
  /*
    Step 9: Chuyển thanh số dương.
  */
  if (x < 0)
  {
    x = x + b;
  }

  return x;
#undef N_MAX
}

int i4_gcd(int i, int j)

/******************************************************************************/
/*
  Purpose:

    I4_GCD tìm ước chung lớn nhất của I và J.

  Discussion:

    Chỉ giá trị tuyệt đối của I và J được xem xét, vì vậy kết quả luôn không âm.

    Nếu I hoặc J bằng 0, I4_GCD sẽ trả về giá trị lớn nhất giữa 1, abs(I) và abs(J).

    Nếu I và J không có ước chung, I4_GCD sẽ trả về 1.

    Trường hợp còn lại, sử dụng thuật toán Euclidean, I4_GCD là ước chung lớn nhất của I và J.

  Parameters:

    Input, int I, J, hai số cần tìm ước chung lớn nhất.

    Output, int I4_GCD, ước chung lớn nhất của I và J.
*/
{
  int ip;
  int iq;
  int ir;
  /*
    Trả về giá trị nếu I hoặc J bằng 0.
  */
  if (i == 0)
  {
    return i4_max(1, abs(j));
  }
  else if (j == 0)
  {
    return i4_max(1, abs(i));
  }
  /*
    Đặt IP là lớn nhất giữa I và J, IQ là nhỏ nhất.
    Điều này giúp chúng ta có thể thay đổi IP và IQ khi cần.

  */
  ip = i4_max(abs(i), abs(j));
  iq = i4_min(abs(i), abs(j));
  /*
    Thực hiện thuật toán Euclidean.
  */
  for (;;)
  {
    ir = ip % iq;

    if (ir == 0)
    {
      break;
    }

    ip = iq;
    iq = ir;
  }

  return iq;
}

int i4_max(int i1, int i2)

/******************************************************************************/
/*
  Purpose:

    I4_MAX trả về giá trị lớn nhất của 2 giá trị truyền vào

  Parameters:

    Input, int I1, I2 là 2 số nguyên cần so sánh

    Output, int I4_MAX là giá trị lớn hơn giữa I1 and I2.
*/
{
  int value;

  if (i2 < i1)
  {
    value = i1;
  }
  else
  {
    value = i2;
  }
  return value;
}

int i4_min(int i1, int i2)

/******************************************************************************/
/*
  Purpose:

    I4_MIN trả về giá trị nhỏ hơn

  Parameters:

    Input, int I1, I2 là 2 số nguyên cần so sánh

    Output, int I4_MIN là giá trị nhỏ hơn giữa I1 and I2.
*/
{
  int value;

  if (i1 < i2)
  {
    value = i1;
  }
  else
  {
    value = i2;
  }
  return value;
}

int i4_sign(int i)

/******************************************************************************/
/*
  Purpose:

    I4_SIGN trả về dấu của số nguyên

  Discussion:

    Dấu của số 0 và số nguyên dương sẽ được xem là +1.
    Dấu của các số nguyên âm sẽ được xem là -1.

  Parameters:

    Input, int I là số nguyên mà dấu của nó cần được xác định.

    Output, int I4_SIGN trả về dấu của số nguyên.
*/
{
  int value;

  if (i < 0)
  {
    value = -1;
  }
  else
  {
    value = 1;
  }
  return value;
}

void lcrg_anbn(int a, int b, int c, int n, int *an, int *bn)

/******************************************************************************/
/*
  Purpose:

    LCRG_ANBN tính toán "mũ N" của LCG.

  Discussion:

    Chúng ta xem xét một bộ sinh số ngẫu nhiên tuyến tính.
    LCRG nhận giá trị SEED làm đầu vào, và trả về giá trị SEED được cập nhật,

      SEED(out) = ( a * SEED(in) + b ) mod c.

    và giá trị ngẫu nhiên ảo tương ứng

      U = SEED(out) / c.

    Trong hầu hết các trường hợp, người dùng hài lòng khi gọi LCRG nhiều lần,
    với việc cập nhật SEED được thực hiện tự động.

    Mục tiêu của hàm này là xác định giá trị của AN và BN
    mô tả LCRG tương đương với N ứng dụng của LCRG ban đầu.

    Một tác dụng của việc này là tính toán số ngẫu nhiên song song.
    Nếu mỗi trong N processors tính toán nhiều giá trị ngẫu nhiên,
    bạn có thể đảm bảo rằng họ làm việc với các giá trị ngẫu nhiên khác nhau
    bằng cách bắt đầu với một giá trị SEED, sử dụng LCRG ban đầu để tạo ra
    N-1 "iterates" đầu tiên của SEED, để bạn có N giá trị "seed",
    và từ nay trở đi, áp dụng N lần của LCRG cho các seed.

    Nếu processor thứ K bắt đầu từ seed thứ K, nó sẽ tính toán mỗi giá trị
    thứ N của chuỗi số ngẫu nhiên ban đầu, lệch đi K.
    Do đó, các processors cá nhân sẽ sử dụng một chuỗi số ngẫu nhiên tốt
    như chuỗi ban đầu, và không lặp lại, và không cần phải giao tiếp.

    Để tính giá trị thứ N của SEED trực tiếp, chúng ta bắt đầu bằng cách bỏ qua
    số học mô-đun và thực hiện trình tự tính toán như sau:

      SEED(0)   =     SEED.
      SEED(1)   = a * SEED      + b
      SEED(2)   = a * SEED(1)   + b = a^2 * SEED           + a * b + b
      SEED(3)   = a * SEED(2)   + b = a^3 * SEED + a^2 * b + a * b + b
      ...
      SEED(N-1) = a * SEED(N-2) + b

      SEED(N) = a * SEED(N-1) + b = a^N * SEED
                                    + ( a^(n-1) + a^(n-2) + ... + a + 1 ) * b

    hoặc, sử dụng geometruc series,

      SEED(N) = a^N * SEED + ( a^N - 1) / ( a - 1 ) * b
              = AN * SEED + BN

    Từ đó, từ bất kỳ SEED nào, chúng ta có thể tính kết quả của N ứng dụng của
    giá trị LCRG nếu ta có thể giải được

      ( a - 1 ) * BN = ( a^N - 1 ) * b in modular arithmetic,

    và tinh giá trị:
      AN = a^N

  Parameters:

    Input, int A, giá trị nhân

    Input, int  B, giá trị cộng

    Input, int  C, cơ sở cho số học mô-đun.
    Đối với 32 bit, giá trị này thường là 2^31 - 1, hoặc 2147483647.

    Input, int N, chỉ số, hoặc số lần mà LCRG cần áp dụng

    Output, int *AN, *BN, số nhân và giá trị gia tăng cho
    LCRG đại diện cho N ứng dụng của LCRG ban đầu.
*/
{
  int am1;
  int anm1tb;
  int ierror;

  if (n < 0)
  {
    printf("\n");
    printf("LCRG_ANBN - Fatal error!\n");
    printf("  Illegal input value of N = %d\n", n);
    exit(1);
  }

  if (c <= 0)
  {
    printf("\n");
    printf("LCRG_ANBN - Fatal error!\n");
    printf("  Illegal input value of C = %d\n", c);
    exit(1);
  }

  if (n == 0) // Không có bước nào
  {
    *an = 1;
    *bn = 0;
  }
  else if (n == 1) // Chỉ có 1 bước
  {
    *an = a;
    *bn = b;
  }
  else
  {
    /*
      Tính A^N.
    */
    *an = power_mod(a, n, c);

    /*
      Giải
        ( a - 1 ) * BN = ( a^N - 1 ) mod B
      tìm BN.
    */
    am1 = a - 1;
    anm1tb = (*an - 1) * b;

    *bn = congruence(am1, c, anm1tb, &ierror);
    // AX = C mod B
    // A = a - 1
    // C = (a^n - 1) * b
    // B = c
    if (ierror)
    {
      printf("\n");
      printf("LCRG_ANBN - Fatal error!\n");
      printf("  An error occurred in the CONGRUENCE routine.\n");
      exit(1);
    }
  }

  return;
}

int lcrg_evaluate(int a, int b, int c, int x)

/******************************************************************************/
/*
  Purpose:

    LCRG_EVALUATE tính LCRG, y = ( A * x + B ) mod C.

  Parameters:

    Input, int A, số nhân.

    Input, int B, giá trị tăng.

    Input, int  C, cơ sở cho số học mô-đun.
    Đối với 32 bit, giá trị này thường là 2^31 - 1, hoặc 2147483647.

    Input, int X, giá trị sử dụng để tính toán.

    Output, int LCRG_EVALUATE, giá trị đã tính toán.
*/
{
  long long int a8;
  long long int b8;
  long long int c8;
  long long int x8;
  int y;
  long long int y8;

  /*
    Để tránh các vấn đề làm tròn, chúng ta cần chuyển đến các số nguyên "có độ chính xác kép".
  */
  a8 = (long long int)a;
  b8 = (long long int)b;
  c8 = (long long int)c;
  x8 = (long long int)x;

  y8 = (a8 * x8 + b8) % c8;

  y = (int)(y8);

  if (y < 0)
  {
    y = y + c;
  }

  return y;
}

int power_mod(int a, int n, int m)

/******************************************************************************/
/*
  Purpose:

    POWER_MOD tính mod ( A^N, M ).

  Discussion:

    Một số thủ thuật lập trình được sử dụng để tăng tốc độ tính toán và để
    cho phép các phép tính trong đó A**N quá lớn để lưu trữ trong một
    lời thật.

    Đầu tiên, để đạt hiệu quả, công suất A**N được tính bằng cách xác định
    khai triển nhị phân của N, sau đó tính toán A, A^2, A^4, v.v.
    bằng cách bình phương lặp đi lặp lại và chỉ nhân những thừa số góp phần
    tạo ra A**N.

    Thứ hai, các sản phẩm trung gian được “sửa đổi” ngay lập tức.
    giữ chúng nhỏ.

    Ví dụ, để tính mod ( A^13, 11 ), ta có thể tính như sau

       13 = 1 + 4 + 8

       A**13 = A * A^4 * A^8

       mod ( A**13, 11 ) = mod ( A, 11 ) * mod ( A^4, 11 ) * mod ( A^8, 11 ).

    Định lý nhỏ Fermat nói rằng nếu P là số nguyên tố và A không chia hết
    cho P thì ( A^(P-1) - 1 ) chia hết cho P.

  Parameters:

    Input, int A, cơ số của biểu thức cần kiểm tra.
    A phải không âm.

    Input, int N, số mũ.
    N phải không âm.

    Input, int M, ước số mà biểu thức được kiểm tra.
    M phải dương.

    Output, int POWER_MOD, phần dư khi chia A^N cho M.
*/
{
  long long int a_square2;
  int d;
  long long int m2;
  int x;
  long long int x2;

  if (a < 0)
  {
    return -1;
  }

  if (m <= 0)
  {
    return -1;
  }

  if (n < 0)
  {
    return -1;
  }

  /*
    A_SQUARE chứa các bình phương liên tiếp của A.
  */
  a_square2 = (long long int)a;
  m2 = (long long int)m;
  x2 = (long long int)1;

  while (0 < n)
  {
    d = n % 2;

    if (d == 1)
    {
      x2 = (x2 * a_square2) % m2;
    }

    a_square2 = (a_square2 * a_square2) % m2;
    n = (n - d) / 2;
  }
  /*
    Đảm bảo rằng 0 <= X.
  */
  while (x2 < 0)
  {
    x2 = x2 + m2;
  }

  x = (int)x2;

  return x;
}

void timestamp(void)

/******************************************************************************/
/*
  Purpose:

    TIMESTAMP in ra giá trị hiện tại YMDHMS date theo kiểu time stamp.

  Example:

    31 May 2001 09:45:54 AM

  Parameters:

    None
*/
{
#define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  time_t now;

  now = time(NULL);
  tm = localtime(&now);

  strftime(time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm);

  printf("%s\n", time_buffer);

  return;
#undef TIME_SIZE
}