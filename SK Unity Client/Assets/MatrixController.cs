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

    private SquareMatrix _matrixA, _matrixB, _matrixC;
    private int _yOut, _xOut;
    private bool _writeResult = false;

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
            _matrixA.Randomize();
            _matrixB.Randomize();
        });

        exitButton.onClick.AddListener(() =>
        {
            Application.Quit();
        });
    }

    private void Update()
    {
        if (_writeResult)
        {
            _matrixC.Write();
            _writeResult = false;
        }
    }

    private void Fill(int size)
    {
        size = Mathf.Clamp(size, 1, 20);
        sizeInputField.text = size.ToString();
        _matrixA = new SquareMatrix(size);
        _matrixB = new SquareMatrix(size);
        _matrixC = new SquareMatrix(size);

        foreach (var child in content.GetComponentsInChildren<Transform>())
        {
            if (child != content)
            {
                Destroy(child.gameObject);
            }
        }

        _matrixA.InputFields = new TMP_InputField[size, size];
        for (int y = 0; y < size; y++)
        {
            var row = Instantiate(rowPrefab, content).transform;
            for (int x = 0; x < size; x++)
            {
                _matrixA.InputFields[y, x] = Instantiate(fieldPrefab, row).GetComponent<TMP_InputField>();
                _matrixA.InputFields[y, x].caretColor = Color.red;
                var c = _matrixA.InputFields[y, x].colors;
                c.normalColor = Color.white;
                _matrixA.InputFields[y, x].colors = c;
            }
        }
        _matrixA.Write();
        _matrixB.InputFields = new TMP_InputField[size, size];
        for (int y = 0; y < size; y++)
        {
            var row = Instantiate(rowPrefab, content).transform;
            for (int x = 0; x < size; x++)
            {
                _matrixB.InputFields[y, x] = Instantiate(fieldPrefab, row).GetComponent<TMP_InputField>();
                var c = _matrixB.InputFields[y, x].colors;
                c.normalColor = Color.yellow;
                _matrixB.InputFields[y, x].colors = c;
            }
        }
        _matrixB.Write();
        _matrixC.InputFields = new TMP_InputField[size, size];
        for (int y = 0; y < size; y++)
        {
            var row = Instantiate(rowPrefab, content).transform;
            for (int x = 0; x < size; x++)
            {
                _matrixC.InputFields[y, x] = Instantiate(fieldPrefab, row).GetComponent<TMP_InputField>();
                var c = _matrixC.InputFields[y, x].colors;
                c.disabledColor = Color.green;
                _matrixC.InputFields[y, x].colors = c;
                _matrixC.InputFields[y, x].interactable = false;
            }
        }
        _matrixC.Write();
    }

    private void Multiply()
    {
        _matrixA.Read();
        _matrixB.Read();

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

        _yOut = 0;
        _xOut = 0;
        p.BeginOutputReadLine();
        p.BeginErrorReadLine();

        p.StandardInput.WriteLine(_matrixA.Size);
        for (int y = 0; y < _matrixA.Size; y++)
        {
            for (int x = 0; x < _matrixA[y].Count; x++)
            {
                string text = _matrixA[y][x].ToString().Replace(',', '.');
                p.StandardInput.Write((x == 0 ? "" : " ") + text);
            }
            p.StandardInput.WriteLine();
        }
        for (int y = 0; y < _matrixB.Size; y++)
        {
            for (int x = 0; x < _matrixB[y].Count; x++)
            {
                string text = _matrixB[y][x].ToString().Replace(',', '.');
                p.StandardInput.Write((x == 0 ? "" : " ") + text);
            }
            p.StandardInput.WriteLine();
        }
        p.StandardInput.Close();
    }

    private void OnCout(object sender, DataReceivedEventArgs e)
    {
        if (e.Data == null)
        {
            return;
        }

        UnityEngine.Debug.Log(e.Data);

        double rowValue = double.TryParse(e.Data.Replace('.', ','), out double value) ? value : 0;

        _matrixC[_yOut][_xOut] = rowValue;

        _xOut++;
        if (_xOut >= _matrixC[_yOut].Count)
        {
            _writeResult = true;
            _xOut = 0;
            _yOut++;
        }
    }

    private void OnCerr(object sender, DataReceivedEventArgs e)
    {
        if (e.Data == null)
        {
            return;
        }

        UnityEngine.Debug.LogError(e.Data);
    }
}
