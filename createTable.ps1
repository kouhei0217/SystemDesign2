if ($Args.Length -eq 0) {
    Write-Output "作成するテーブル名を引数に指定してください"
    exit
}

$databaseCSV = Import-Csv database.csv
$database = @()
$databaseRow = @()
$i = 1
foreach ($databaseCSVRow in $databaseCSV) {
    for ($i = 1; $i -le (Get-Content -Path database.csv -TotalCount 1).Split(",").Count; $i++) {
        $databaseRow += $databaseCSVRow.$i
    }
    $database += , $databaseRow
    $databaseRow = @()
}

$sql = "CREATE TABLE $Args("
$primaryKey = ""
for ($i = 0; $i -lt $database.Length; $i++) {
    if ($database[$i][1] -eq $Args[0]) {
        for ($j = 1; $j -lt $database[0].Length; $j++) {
            if ($database[$i + 1][$j] -ne "") {
                $sql += $database[$i + 1][$j] + " " + $database[$i + 2][$j]
                if ($database[$i + 3][$j] -eq "NO") {
                    $sql += " NOT NULL"
                }
                if ($database[$i + 4][$j] -ne "") {
                    if ($database[$i + 4][$j] -eq "PRIMARY") {
                        if ($primaryKey -ne "") {
                            $primaryKey += ", "
                        }
                        $primaryKey += $database[$i + 1][$j]
                    }
                    else {
                        $sql += " " + $database[$i + 4][$j]
                    }
                }
                if ($database[$i + 5][$j] -ne "NULL") {
                    $sql += " DEFAULT " + $database[$i + 5][$j]
                }
                if ($database[$i + 6][$j] -ne "") {
                    $sql += " " + $database[$i + 6][$j]
                }
                if (($j -ne $database[0].Length - 1) -or ($primaryKey -ne "")) {
                    $sql += ", "
                }
            }
        }
        if ($primaryKey -ne "") {
            $sql += "PRIMARY KEY(" + $primaryKey + ")"
        }
        $sql += ");"
        Write-Output $sql
        exit
    }
    elseif ($i -eq $database.Length - 1) {
        Write-Output "テーブルが見つかりませんでした"
        exit
    }
}
