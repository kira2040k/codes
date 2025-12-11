async function getIamCredentials() {
  const baseUrl = "http://169.254.169.254/latest";

  // 1) Get IMDSv2 token
  const tokenRes = await fetch(`${baseUrl}/api/token`, {
    method: "PUT",
    headers: {
      "X-aws-ec2-metadata-token-ttl-seconds": "21600",
    },
  });

  const token = await tokenRes.text();

  // 2) Get IAM role name
  const roleRes = await fetch(`${baseUrl}/meta-data/iam/security-credentials/`, {
    headers: {
      "X-aws-ec2-metadata-token": token,
    },
  });

  const roleName = await roleRes.text();

  // 3) Get IAM credentials for that role
  const credsRes = await fetch(
    `${baseUrl}/meta-data/iam/security-credentials/${roleName}`,
    {
      headers: {
        "X-aws-ec2-metadata-token": token,
      },
    }
  );

  const credentials = await credsRes.json();
  return credentials;
}

// Run it
getIamCredentials()
  .then(console.log)
  .catch(console.error);
