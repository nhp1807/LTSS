#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int i4_gcd(int i, int j);
int i4_max(int i1, int i2);
int i4_min(int i1, int i2);

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

int main()
{
    // int y = i4_gcd(550, 1755);

    printf("y = %d\n", -1 % 3);

    return 0;
}