Private Sub Worksheet_Change(ByVal Target As Range)
    ' 如果變動範圍同時不包含 L2 和 L3 就跳出，不做任何事
    If Intersect(Target, Me.Range("L2")) Is Nothing And Intersect(Target, Me.Range("L3")) Is Nothing Then Exit Sub

    Application.EnableEvents = False

    Dim oldText As String
    Dim newText As String
    Dim targetSheet As Worksheet
    Dim cell As Range
    Dim f As String

    ' 如果是 L2 變動
    If Not Intersect(Target, Me.Range("L2")) Is Nothing Then
        oldText = Me.Range("K2").Value
        newText = Me.Range("L2").Value

        Set targetSheet = ThisWorkbook.Sheets("callPut（W）")

        For Each cell In targetSheet.UsedRange
            If cell.HasFormula Then
                f = cell.Formula
                If InStr(f, oldText) > 0 Then
                    cell.Formula = Replace(f, oldText, newText)
                End If
            ElseIf VarType(cell.Value) = vbString Then
                If InStr(cell.Value, oldText) > 0 Then
                    cell.Value = Replace(cell.Value, oldText, newText)
                End If
            End If
        Next cell

        MsgBox "已完成『callPut（W）』整張工作表中「" & oldText & "」 → 「" & newText & "」的替換。", vbInformation
    End If

    ' 如果是 L3 變動
    If Not Intersect(Target, Me.Range("L3")) Is Nothing Then
        oldText = Me.Range("K3").Value
        newText = Me.Range("L3").Value

        Set targetSheet = ThisWorkbook.Sheets("callPut（F）")

        For Each cell In targetSheet.UsedRange
            If cell.HasFormula Then
                f = cell.Formula
                If InStr(f, oldText) > 0 Then
                    cell.Formula = Replace(f, oldText, newText)
                End If
            ElseIf VarType(cell.Value) = vbString Then
                If InStr(cell.Value, oldText) > 0 Then
                    cell.Value = Replace(cell.Value, oldText, newText)
                End If
            End If
        Next cell

        MsgBox "已完成『callPut（F）』整張工作表中「" & oldText & "」 → 「" & newText & "」的替換。", vbInformation
    End If

    Application.EnableEvents = True
End Sub
