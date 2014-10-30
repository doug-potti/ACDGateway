namespace AGMIR
{
    partial class mainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.txtReqId = new System.Windows.Forms.TextBox();
            this.txtMediaType = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.txtBusType = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtCustLv = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.btnDis = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.txtIp = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.txtTransId = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.txtSrcId = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.txtDstId = new System.Windows.Forms.TextBox();
            this.btnCancelTrans = new System.Windows.Forms.Button();
            this.btnTrans = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.panel1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 45);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "请求标识";
            // 
            // txtReqId
            // 
            this.txtReqId.Location = new System.Drawing.Point(71, 42);
            this.txtReqId.Name = "txtReqId";
            this.txtReqId.Size = new System.Drawing.Size(100, 21);
            this.txtReqId.TabIndex = 1;
            this.txtReqId.Text = "1002";
            // 
            // txtMediaType
            // 
            this.txtMediaType.Location = new System.Drawing.Point(244, 42);
            this.txtMediaType.Name = "txtMediaType";
            this.txtMediaType.Size = new System.Drawing.Size(100, 21);
            this.txtMediaType.TabIndex = 3;
            this.txtMediaType.Text = "weixin";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(185, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "媒体类型";
            // 
            // txtBusType
            // 
            this.txtBusType.Location = new System.Drawing.Point(71, 69);
            this.txtBusType.Name = "txtBusType";
            this.txtBusType.Size = new System.Drawing.Size(100, 21);
            this.txtBusType.TabIndex = 5;
            this.txtBusType.Text = "consult";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 72);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "业务类型";
            // 
            // txtCustLv
            // 
            this.txtCustLv.Location = new System.Drawing.Point(244, 69);
            this.txtCustLv.Name = "txtCustLv";
            this.txtCustLv.Size = new System.Drawing.Size(100, 21);
            this.txtCustLv.TabIndex = 7;
            this.txtCustLv.Text = "3";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(185, 72);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 6;
            this.label4.Text = "客户等级";
            // 
            // btnDis
            // 
            this.btnDis.Location = new System.Drawing.Point(43, 107);
            this.btnDis.Name = "btnDis";
            this.btnDis.Size = new System.Drawing.Size(118, 28);
            this.btnDis.TabIndex = 8;
            this.btnDis.Text = "请求分配座席";
            this.btnDis.UseVisualStyleBackColor = true;
            this.btnDis.Click += new System.EventHandler(this.btnDis_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(206, 107);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(126, 28);
            this.button1.TabIndex = 9;
            this.button1.Text = "取消请求分配座席";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // txtIp
            // 
            this.txtIp.Location = new System.Drawing.Point(71, 12);
            this.txtIp.Name = "txtIp";
            this.txtIp.Size = new System.Drawing.Size(100, 21);
            this.txtIp.TabIndex = 11;
            this.txtIp.Text = "127.0.0.1";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(23, 15);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(29, 12);
            this.label5.TabIndex = 10;
            this.label5.Text = "AGIP";
            // 
            // txtPort
            // 
            this.txtPort.Location = new System.Drawing.Point(244, 12);
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(100, 21);
            this.txtPort.TabIndex = 13;
            this.txtPort.Text = "8081";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(192, 15);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(41, 12);
            this.label6.TabIndex = 12;
            this.label6.Text = "AGPort";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(289, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(352, 262);
            this.panel1.TabIndex = 14;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtIp);
            this.groupBox1.Controls.Add(this.txtPort);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.txtReqId);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.txtMediaType);
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.btnDis);
            this.groupBox1.Controls.Add(this.txtBusType);
            this.groupBox1.Controls.Add(this.txtCustLv);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(352, 143);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnCancelTrans);
            this.groupBox2.Controls.Add(this.btnTrans);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.txtTransId);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.txtSrcId);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.txtDstId);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Location = new System.Drawing.Point(0, 143);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(352, 119);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 23);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 6;
            this.label7.Text = "请求标识";
            // 
            // txtTransId
            // 
            this.txtTransId.Location = new System.Drawing.Point(71, 20);
            this.txtTransId.Name = "txtTransId";
            this.txtTransId.Size = new System.Drawing.Size(100, 21);
            this.txtTransId.TabIndex = 7;
            this.txtTransId.Text = "1003";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(185, 23);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(41, 12);
            this.label8.TabIndex = 8;
            this.label8.Text = "源座席";
            // 
            // txtSrcId
            // 
            this.txtSrcId.Location = new System.Drawing.Point(244, 20);
            this.txtSrcId.Name = "txtSrcId";
            this.txtSrcId.Size = new System.Drawing.Size(100, 21);
            this.txtSrcId.TabIndex = 9;
            this.txtSrcId.Text = "4501";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(23, 50);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(29, 12);
            this.label9.TabIndex = 10;
            this.label9.Text = "目的";
            // 
            // txtDstId
            // 
            this.txtDstId.Location = new System.Drawing.Point(71, 47);
            this.txtDstId.Name = "txtDstId";
            this.txtDstId.Size = new System.Drawing.Size(100, 21);
            this.txtDstId.TabIndex = 11;
            this.txtDstId.Text = "4502";
            // 
            // btnCancelTrans
            // 
            this.btnCancelTrans.Location = new System.Drawing.Point(206, 79);
            this.btnCancelTrans.Name = "btnCancelTrans";
            this.btnCancelTrans.Size = new System.Drawing.Size(126, 28);
            this.btnCancelTrans.TabIndex = 13;
            this.btnCancelTrans.Text = "取消请求转移座席";
            this.btnCancelTrans.UseVisualStyleBackColor = true;
            // 
            // btnTrans
            // 
            this.btnTrans.Location = new System.Drawing.Point(43, 79);
            this.btnTrans.Name = "btnTrans";
            this.btnTrans.Size = new System.Drawing.Size(118, 28);
            this.btnTrans.TabIndex = 12;
            this.btnTrans.Text = "请求转移座席";
            this.btnTrans.UseVisualStyleBackColor = true;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.richTextBox1);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(289, 262);
            this.panel2.TabIndex = 15;
            // 
            // richTextBox1
            // 
            this.richTextBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBox1.Location = new System.Drawing.Point(0, 0);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(289, 262);
            this.richTextBox1.TabIndex = 0;
            this.richTextBox1.Text = "";
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(641, 262);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "mainForm";
            this.Text = "AGMIR";
            this.panel1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtReqId;
        private System.Windows.Forms.TextBox txtMediaType;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtBusType;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtCustLv;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnDis;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox txtIp;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnCancelTrans;
        private System.Windows.Forms.Button btnTrans;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtTransId;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtSrcId;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox txtDstId;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.RichTextBox richTextBox1;
    }
}

