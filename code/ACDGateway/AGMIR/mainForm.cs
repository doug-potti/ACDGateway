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
                return string.Format("http://{0}:{1}",
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

             WebClient wc = new WebClient();
             Uri uri = new Uri(string.Format("{0}/DistributeMultimediaCall.html?Id={1}&Mediatype={2}&Businesstype={3}&Customlevel={4}",
                                              AGUrl,
                                              txtReqId.Text,
                                              txtMediaType.Text,
                                              txtBusType.Text,
                                              txtCustLv.Text), UriKind.RelativeOrAbsolute);

            wc.OpenReadCompleted += new OpenReadCompletedEventHandler(wc_OpenReadCompleted);
            wc.OpenReadAsync(uri);


        }

        void wc_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
        {

            if (e.Error == null)
            {
                var streamReader = new StreamReader(e.Result);
                var result = streamReader.ReadToEnd();
                rtInfo.Text = rtInfo.Text + result + "\n";
            }
            else
            {
                rtInfo.Text = rtInfo.Text + e.Error + "\n";
            }

        }
        
        private void button1_Click(object sender, EventArgs e)
        {
            WebClient wc = new WebClient();
            Uri uri = new Uri(string.Format("{0}/CancelDistributeMultimediaCall.html?Id={1}",
                                             AGUrl,
                                             txtReqId.Text), UriKind.RelativeOrAbsolute);
            wc.OpenReadCompleted += new OpenReadCompletedEventHandler(wc_OpenReadCompleted);
            wc.OpenReadAsync(uri);
        }

        private void btnTrans_Click(object sender, EventArgs e)
        {
            WebClient wc = new WebClient();
            Uri uri = new Uri(string.Format("{0}/Transfer.html?Id={1}&Srcagentid={2}&Destinationagentid={3}",
                                             AGUrl,
                                             txtTransId.Text,
                                             txtSrcId.Text,
                                             txtDstId.Text), UriKind.RelativeOrAbsolute);

            wc.OpenReadCompleted += new OpenReadCompletedEventHandler(wc_OpenReadCompleted);
            wc.OpenReadAsync(uri);
        }

        private void btnCancelTrans_Click(object sender, EventArgs e)
        {
            WebClient wc = new WebClient();
            Uri uri = new Uri(string.Format("{0}/CancelTransfer.html?Id={1}",
                                             AGUrl,
                                             txtTransId.Text), UriKind.RelativeOrAbsolute);
            wc.OpenReadCompleted += new OpenReadCompletedEventHandler(wc_OpenReadCompleted);
            wc.OpenReadAsync(uri);
        }
    }
}
