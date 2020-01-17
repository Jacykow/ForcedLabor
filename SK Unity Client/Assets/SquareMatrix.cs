using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class SquareMatrix : List<List<double>>
{
    public int Size { get; }
    public TMP_InputField[,] InputFields { get; set; }

    public SquareMatrix(int size)
    {
        Size = size;
        for (int y = 0; y < size; y++)
        {
            var row = new List<double>(size);
            for (int x = 0; x < size; x++)
            {
                row.Add(0.0);
            }
            Add(row);
        }
    }

    public void Randomize()
    {
        foreach (var row in this)
        {
            for (int x = 0; x < row.Count; x++)
            {
                row[x] = 0.01 * Random.Range(-10000, 10000);
            }
        }
        Write();
    }

    public void Read()
    {
        for (int y = 0; y < Size; y++)
        {
            for (int x = 0; x < Size; x++)
            {
                this[y][x] = double.TryParse(InputFields[y, x].text.Replace('.', ','), out double value) ? value : 0.0;
            }
        }
    }

    public void Write()
    {
        for (int y = 0; y < Size; y++)
        {
            for (int x = 0; x < Size; x++)
            {
                Write(x, y);
            }
        }
    }

    public void Write(int x, int y)
    {
        InputFields[y, x].text = this[y][x].ToString().Replace(',', '.');
    }
}
