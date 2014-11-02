using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;

namespace AGMIR
{
    public partial class mainForm : Form
    {
        
        public string AGUrl
        {
            get
            {
                return string.Format("http://{0}:{1}/DistributeMultimediaCall.html",
                                     txtIp.Text,
                                     txtPort.Text);
            }
        }
        public mainForm()
        {
            InitializeComponent();

        }

        private void btnDis_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(txtReqId.Text) ||
                string.IsNullOrWhiteSpace(txtMediaType.Text) ||
                string.IsNullOrWhiteSpace(txtBusType.Text) ||
                string.IsNullOrWhiteSpace(txtCustLv.Text))
            {
                return;
            }
            
            Uri uri = new Uri(string.Format("{0}?Id={1}&Mediatype={2}&Businesstype={3}&Customlevel={4}",
                                             AGUrl,
                                             txtReqId.Text,
                                             txtMediaType.Text,
                                             txtBusType.Text,
                                             txtCustLv.Text), UriKind.RelativeOrAbsolute);

            WebClient wc = new WebClient();
            wc.OpenReadCompleted += new OpenReadCompletedEventHandler(wc_OpenReadCompleted);
            wc.OpenReadAsync(uri);
        }

        void wc_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
        {
            StreamReader read = new StreamReader(e.Result);
            string result = read.ReadToEnd();
            rtInfo.Text = rtInfo.Text + result + "\n";
        }
        
        private void button1_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(txtReqId.Text))
            {
                return;
            }
        }
    }
}
