using System.Diagnostics;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class MatrixController : MonoBehaviour
{
    public TMP_InputField sizeInputField;
    public Transform content;
    public GameObject rowPrefab, fieldPrefab;
    public Button calculateButton, randomizeButton, exitButton;

    private SquareMatrix matrixA, matrixB, matrixC;

    private void Start()
    {
        sizeInputField.onEndEdit.AddListener(text =>
        {
            Fill(int.Parse(text));
        });

        calculateButton.onClick.AddListener(() =>
        {
            Multiply();
        });

        randomizeButton.onClick.AddListener(() =>
        {
            matrixA.Randomize();
            matrixB.Randomize();
        });

        exitButton.onClick.AddListener(() =>
        {
            Application.Quit();
        });
    }

    private void Fill(int size)
    {
        size = Mathf.Clamp(size, 1, 20);
        sizeInputField.text = size.ToString();
        matrixA = new SquareMatrix(size);
        matrixB = new SquareMatrix(size);
        matrixC = new SquareMatrix(size);

        foreach (var child in content.GetComponentsInChildren<Transform>())
        {
            if (child != content)
            {
                Destroy(child.gameObject);
            }
        }

        matrixA.InputFields = new TMP_InputField[size, size];
        for (int y = 0; y < size; y++)
        {
            var row = Instantiate(rowPrefab, content).transform;
            for (int x = 0; x < size; x++)
            {
                matrixA.InputFields[y, x] = Instantiate(fieldPrefab, row).GetComponent<TMP_InputField>();
                matrixA.InputFields[y, x].caretColor = Color.red;
                var c = matrixA.InputFields[y, x].colors;
                c.normalColor = Color.white;
                matrixA.InputFields[y, x].colors = c;
            }
        }
        matrixA.Write();
        matrixB.InputFields = new TMP_InputField[size, size];
        for (int y = 0; y < size; y++)
        {
            var row = Instantiate(rowPrefab, content).transform;
            for (int x = 0; x < size; x++)
            {
                matrixB.InputFields[y, x] = Instantiate(fieldPrefab, row).GetComponent<TMP_InputField>();
                var c = matrixB.InputFields[y, x].colors;
                c.normalColor = Color.yellow;
                matrixB.InputFields[y, x].colors = c;
            }
        }
        matrixB.Write();
        matrixC.InputFields = new TMP_InputField[size, size];
        for (int y = 0; y < size; y++)
        {
            var row = Instantiate(rowPrefab, content).transform;
            for (int x = 0; x < size; x++)
            {
                matrixC.InputFields[y, x] = Instantiate(fieldPrefab, row).GetComponent<TMP_InputField>();
                var c = matrixC.InputFields[y, x].colors;
                c.disabledColor = Color.green;
                matrixC.InputFields[y, x].colors = c;
                matrixC.InputFields[y, x].interactable = false;
            }
        }
        matrixC.Write();
    }

    private void Multiply()
    {
        matrixA.Read();
        matrixB.Read();

        var p = new Process();
        p.StartInfo.WorkingDirectory = Application.streamingAssetsPath;
        p.StartInfo.FileName = p.StartInfo.WorkingDirectory + "/Client Connector.exe";
        p.StartInfo.UseShellExecute = false;
        p.StartInfo.RedirectStandardInput = true;
        p.StartInfo.RedirectStandardOutput = true;
        p.StartInfo.RedirectStandardError = true;

        p.OutputDataReceived += OnCout;
        p.ErrorDataReceived += OnCerr;

        p.Start();
        p.StandardInput.WriteLine(matrixA.Size);
        for (int y = 0; y < matrixA.Size; y++)
        {
            for (int x = 0; x < matrixA[y].Count; x++)
            {
                string text = matrixA[y][x].ToString().Replace(',', '.');
                p.StandardInput.Write((x == 0 ? "" : " ") + text);
            }
            p.StandardInput.WriteLine();
        }
        for (int y = 0; y < matrixB.Size; y++)
        {
            for (int x = 0; x < matrixB[y].Count; x++)
            {
                string text = matrixB[y][x].ToString().Replace(',', '.');
                p.StandardInput.Write((x == 0 ? "" : " ") + text);
            }
            p.StandardInput.WriteLine();
        }
        p.StandardInput.Close();

        xOut = 0;
        yOut = 0;

        p.BeginErrorReadLine();
        p.BeginOutputReadLine();

        //p.Close();

        matrixC.Write();
    }

    private int xOut, yOut;

    private void OnCout(object sender, DataReceivedEventArgs e)
    {
        UnityEngine.Debug.Log(e.Data);

        /*
        for (int y = 0; y < matrixC.Size; y++)
        {
            string line = p.StandardError.ReadLine();
            double[] rowValues = line.Split(' ').Select(text => double.TryParse(text.Replace('.', ','), out double value) ? value : 0).ToArray();
            for (int x = 0; x < matrixC[y].Count; x++)
            {
                matrixC[y][x] = rowValues[x];
            }
        }
        */
    }

    private void OnCerr(object sender, DataReceivedEventArgs e)
    {
        UnityEngine.Debug.LogError(e.Data);
    }
}
